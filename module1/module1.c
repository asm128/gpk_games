#include <Python.h>

/*
 * Implements an example function.
 */
PyDoc_STRVAR(module1_example_doc, "example(obj, number)\
\
Example function");

PyObject *module1_example(PyObject *self, PyObject *args, PyObject *kwargs) {
    /* Shared references that do not need Py_DECREF before returning. */
    PyObject *obj = NULL;
    int number = 0;

    /* Parse positional and keyword arguments */
    static char* keywords[] = { "obj", "number", NULL };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", keywords, &obj, &number)) {
        return NULL;
    }

    /* Function implementation starts here */

    if (number < 0) {
        PyErr_SetObject(PyExc_ValueError, obj);
        return NULL;    /* return NULL indicates error */
    }

    Py_RETURN_NONE;
}

/*
 * List of functions to add to module1 in exec_module1().
 */
static PyMethodDef module1_functions[] = {
    { "example", (PyCFunction)module1_example, METH_VARARGS | METH_KEYWORDS, module1_example_doc },
    { NULL, NULL, 0, NULL } /* marks end of array */
};

/*
 * Initialize module1. May be called multiple times, so avoid
 * using static state.
 */
int exec_module1(PyObject *module) {
    PyModule_AddFunctions(module, module1_functions);

    PyModule_AddStringConstant(module, "__author__", "Gontrill");
    PyModule_AddStringConstant(module, "__version__", "1.0.0");
    PyModule_AddIntConstant(module, "year", 2022);

    return 0; /* success */
}

/*
 * Documentation for module1.
 */
PyDoc_STRVAR(module1_doc, "The module1 module");


static PyModuleDef_Slot module1_slots[] = {
    { Py_mod_exec, exec_module1 },
    { 0, NULL }
};

static PyModuleDef module1_def = {
    PyModuleDef_HEAD_INIT,
    "module1",
    module1_doc,
    0,              /* m_size */
    NULL,           /* m_methods */
    module1_slots,
    NULL,           /* m_traverse */
    NULL,           /* m_clear */
    NULL,           /* m_free */
};

PyMODINIT_FUNC PyInit_module1() {
    return PyModuleDef_Init(&module1_def);
}
