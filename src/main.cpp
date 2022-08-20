#include <stdio.h>
#include <pybind11/pybind11.h>
#include <gdelta.h>
#include <gdelta_internal.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

using namespace pybind11::literals;
namespace py = pybind11;

inline std::basic_string_view<uint8_t> bytes_view(py::bytes bytes) {
    py::buffer_info info(py::buffer(bytes).request());
    return { static_cast<uint8_t*>(info.ptr), static_cast<uint64_t>(info.size) };
}

template<typename... Args>
std::string Sprintf(const char *fmt, Args... args)
{
    const size_t n = snprintf(nullptr, 0, fmt, args...);
    std::vector<char> buf(n+1);
    snprintf(buf.data(), n+1, fmt, args...);
    return std::string(buf.data());
}

PYBIND11_MODULE(gdelta, m) {
    m.doc() = R"pbdoc(
        GDelta Python Bindings 
        -----------------------

        .. currentmodule:: gdelta 

        .. autosummary::
           :toctree: _generate

           encode
           decode
    )pbdoc";

    m.def("describe", [](py::bytes deltaB) {
	auto delta = bytes_view(deltaB);
	ReadOnlyBufferStreamDescriptor deltaStream = {delta.data(), 0, delta.size()};

	const uint64_t instructionLength = read_varint(deltaStream);
	const uint64_t instOffset = deltaStream.cursor;
	std::string out = Sprintf("Instruction Length: %lld bytes\nData Length: %lld bytes\n", instructionLength, delta.size() - instructionLength - instOffset);

	out += Sprintf("Units:\n");
	DeltaUnitMem unit = {};
	while (deltaStream.cursor < instructionLength + instOffset) {
	    read_unit(deltaStream, unit);
	    if (unit.flag){
	        out += Sprintf("- TakeBaseAtOffset(offset=%d,length=%d)\n", unit.offset, unit.length);
	    } else {
		out += Sprintf("- TakeDelta(length=%d)\n", unit.length);
	    }
	}
	return out;
    });

    m.def("encode", [](py::bytes baseB, py::bytes newB) {
	auto base = bytes_view(baseB);
	auto _new = bytes_view(newB);

	uint8_t* deltaOut = nullptr;
	uint32_t deltaSize;
	gencode(_new.data(), _new.size(), base.data(), base.size(), &deltaOut, &deltaSize); 
	
	PyObject *ret = Py_BuildValue("y#", deltaOut, deltaSize);
	free(deltaOut);
	return py::reinterpret_steal<py::object>(ret);
    }, R"pbdoc(
    	Encode a base & new into a delta buffer
    )pbdoc", py::arg("base"), py::arg("new"));

    m.def("decode", [](py::bytes baseB, py::bytes deltaB) { 
	auto base = bytes_view(baseB);
	auto delta = bytes_view(deltaB);

	uint8_t* newOut = nullptr;
	uint32_t newSize;
	gdecode(delta.data(), delta.size(), base.data(), base.size(), &newOut, &newSize);

	PyObject *ret = Py_BuildValue("y#", newOut, newSize);
	free(newOut);
	return py::reinterpret_steal<py::object>(ret);
    }, R"pbdoc(
    	Decode a base & delta into the new data

	! Warning: gdelta currently does not have a checksum, corrupted delta/base will return a corrupted data without warning !.
    )pbdoc", py::arg("base"), py::arg("delta"));

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
