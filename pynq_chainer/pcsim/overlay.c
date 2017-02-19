#define _CFFI_
#include <Python.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

/* This part is from file 'cffi/parse_c_type.h'.  It is copied at the
   beginning of C sources generated by CFFI's ffi.set_source(). */

typedef void *_cffi_opcode_t;

#define _CFFI_OP(opcode, arg)   (_cffi_opcode_t)(opcode | (((uintptr_t)(arg)) << 8))
#define _CFFI_GETOP(cffi_opcode)    ((unsigned char)(uintptr_t)cffi_opcode)
#define _CFFI_GETARG(cffi_opcode)   (((intptr_t)cffi_opcode) >> 8)

#define _CFFI_OP_PRIMITIVE       1
#define _CFFI_OP_POINTER         3
#define _CFFI_OP_ARRAY           5
#define _CFFI_OP_OPEN_ARRAY      7
#define _CFFI_OP_STRUCT_UNION    9
#define _CFFI_OP_ENUM           11
#define _CFFI_OP_FUNCTION       13
#define _CFFI_OP_FUNCTION_END   15
#define _CFFI_OP_NOOP           17
#define _CFFI_OP_BITFIELD       19
#define _CFFI_OP_TYPENAME       21
#define _CFFI_OP_CPYTHON_BLTN_V 23   // varargs
#define _CFFI_OP_CPYTHON_BLTN_N 25   // noargs
#define _CFFI_OP_CPYTHON_BLTN_O 27   // O  (i.e. a single arg)
#define _CFFI_OP_CONSTANT       29
#define _CFFI_OP_CONSTANT_INT   31
#define _CFFI_OP_GLOBAL_VAR     33
#define _CFFI_OP_DLOPEN_FUNC    35
#define _CFFI_OP_DLOPEN_CONST   37
#define _CFFI_OP_GLOBAL_VAR_F   39
#define _CFFI_OP_EXTERN_PYTHON  41

#define _CFFI_PRIM_VOID          0
#define _CFFI_PRIM_BOOL          1
#define _CFFI_PRIM_CHAR          2
#define _CFFI_PRIM_SCHAR         3
#define _CFFI_PRIM_UCHAR         4
#define _CFFI_PRIM_SHORT         5
#define _CFFI_PRIM_USHORT        6
#define _CFFI_PRIM_INT           7
#define _CFFI_PRIM_UINT          8
#define _CFFI_PRIM_LONG          9
#define _CFFI_PRIM_ULONG        10
#define _CFFI_PRIM_LONGLONG     11
#define _CFFI_PRIM_ULONGLONG    12
#define _CFFI_PRIM_FLOAT        13
#define _CFFI_PRIM_DOUBLE       14
#define _CFFI_PRIM_LONGDOUBLE   15

#define _CFFI_PRIM_WCHAR        16
#define _CFFI_PRIM_INT8         17
#define _CFFI_PRIM_UINT8        18
#define _CFFI_PRIM_INT16        19
#define _CFFI_PRIM_UINT16       20
#define _CFFI_PRIM_INT32        21
#define _CFFI_PRIM_UINT32       22
#define _CFFI_PRIM_INT64        23
#define _CFFI_PRIM_UINT64       24
#define _CFFI_PRIM_INTPTR       25
#define _CFFI_PRIM_UINTPTR      26
#define _CFFI_PRIM_PTRDIFF      27
#define _CFFI_PRIM_SIZE         28
#define _CFFI_PRIM_SSIZE        29
#define _CFFI_PRIM_INT_LEAST8   30
#define _CFFI_PRIM_UINT_LEAST8  31
#define _CFFI_PRIM_INT_LEAST16  32
#define _CFFI_PRIM_UINT_LEAST16 33
#define _CFFI_PRIM_INT_LEAST32  34
#define _CFFI_PRIM_UINT_LEAST32 35
#define _CFFI_PRIM_INT_LEAST64  36
#define _CFFI_PRIM_UINT_LEAST64 37
#define _CFFI_PRIM_INT_FAST8    38
#define _CFFI_PRIM_UINT_FAST8   39
#define _CFFI_PRIM_INT_FAST16   40
#define _CFFI_PRIM_UINT_FAST16  41
#define _CFFI_PRIM_INT_FAST32   42
#define _CFFI_PRIM_UINT_FAST32  43
#define _CFFI_PRIM_INT_FAST64   44
#define _CFFI_PRIM_UINT_FAST64  45
#define _CFFI_PRIM_INTMAX       46
#define _CFFI_PRIM_UINTMAX      47

#define _CFFI__NUM_PRIM         48
#define _CFFI__UNKNOWN_PRIM           (-1)
#define _CFFI__UNKNOWN_FLOAT_PRIM     (-2)
#define _CFFI__UNKNOWN_LONG_DOUBLE    (-3)

#define _CFFI__IO_FILE_STRUCT         (-1)


struct _cffi_global_s {
    const char *name;
    void *address;
    _cffi_opcode_t type_op;
    void *size_or_direct_fn;  // OP_GLOBAL_VAR: size, or 0 if unknown
                              // OP_CPYTHON_BLTN_*: addr of direct function
};

struct _cffi_getconst_s {
    unsigned long long value;
    const struct _cffi_type_context_s *ctx;
    int gindex;
};

struct _cffi_struct_union_s {
    const char *name;
    int type_index;          // -> _cffi_types, on a OP_STRUCT_UNION
    int flags;               // _CFFI_F_* flags below
    size_t size;
    int alignment;
    int first_field_index;   // -> _cffi_fields array
    int num_fields;
};
#define _CFFI_F_UNION         0x01   // is a union, not a struct
#define _CFFI_F_CHECK_FIELDS  0x02   // complain if fields are not in the
                                     // "standard layout" or if some are missing
#define _CFFI_F_PACKED        0x04   // for CHECK_FIELDS, assume a packed struct
#define _CFFI_F_EXTERNAL      0x08   // in some other ffi.include()
#define _CFFI_F_OPAQUE        0x10   // opaque

struct _cffi_field_s {
    const char *name;
    size_t field_offset;
    size_t field_size;
    _cffi_opcode_t field_type_op;
};

struct _cffi_enum_s {
    const char *name;
    int type_index;          // -> _cffi_types, on a OP_ENUM
    int type_prim;           // _CFFI_PRIM_xxx
    const char *enumerators; // comma-delimited string
};

struct _cffi_typename_s {
    const char *name;
    int type_index;   /* if opaque, points to a possibly artificial
                         OP_STRUCT which is itself opaque */
};

struct _cffi_type_context_s {
    _cffi_opcode_t *types;
    const struct _cffi_global_s *globals;
    const struct _cffi_field_s *fields;
    const struct _cffi_struct_union_s *struct_unions;
    const struct _cffi_enum_s *enums;
    const struct _cffi_typename_s *typenames;
    int num_globals;
    int num_struct_unions;
    int num_enums;
    int num_typenames;
    const char *const *includes;
    int num_types;
    int flags;      /* future extension */
};

struct _cffi_parse_info_s {
    const struct _cffi_type_context_s *ctx;
    _cffi_opcode_t *output;
    unsigned int output_size;
    size_t error_location;
    const char *error_message;
};

struct _cffi_externpy_s {
    const char *name;
    size_t size_of_result;
    void *reserved1, *reserved2;
};

#ifdef _CFFI_INTERNAL
static int parse_c_type(struct _cffi_parse_info_s *info, const char *input);
static int search_in_globals(const struct _cffi_type_context_s *ctx,
                             const char *search, size_t search_len);
static int search_in_struct_unions(const struct _cffi_type_context_s *ctx,
                                   const char *search, size_t search_len);
#endif

/* this block of #ifs should be kept exactly identical between
   c/_cffi_backend.c, cffi/vengine_cpy.py, cffi/vengine_gen.py
   and cffi/_cffi_include.h */
#if defined(_MSC_VER)
# include <malloc.h>   /* for alloca() */
# if _MSC_VER < 1600   /* MSVC < 2010 */
   typedef __int8 int8_t;
   typedef __int16 int16_t;
   typedef __int32 int32_t;
   typedef __int64 int64_t;
   typedef unsigned __int8 uint8_t;
   typedef unsigned __int16 uint16_t;
   typedef unsigned __int32 uint32_t;
   typedef unsigned __int64 uint64_t;
   typedef __int8 int_least8_t;
   typedef __int16 int_least16_t;
   typedef __int32 int_least32_t;
   typedef __int64 int_least64_t;
   typedef unsigned __int8 uint_least8_t;
   typedef unsigned __int16 uint_least16_t;
   typedef unsigned __int32 uint_least32_t;
   typedef unsigned __int64 uint_least64_t;
   typedef __int8 int_fast8_t;
   typedef __int16 int_fast16_t;
   typedef __int32 int_fast32_t;
   typedef __int64 int_fast64_t;
   typedef unsigned __int8 uint_fast8_t;
   typedef unsigned __int16 uint_fast16_t;
   typedef unsigned __int32 uint_fast32_t;
   typedef unsigned __int64 uint_fast64_t;
   typedef __int64 intmax_t;
   typedef unsigned __int64 uintmax_t;
# else
#  include <stdint.h>
# endif
# if _MSC_VER < 1800   /* MSVC < 2013 */
   typedef unsigned char _Bool;
# endif
#else
# include <stdint.h>
# if (defined (__SVR4) && defined (__sun)) || defined(_AIX) || defined(__hpux)
#  include <alloca.h>
# endif
#endif

#ifdef __GNUC__
# define _CFFI_UNUSED_FN  __attribute__((unused))
#else
# define _CFFI_UNUSED_FN  /* nothing */
#endif

/**********  CPython-specific section  **********/
#ifndef PYPY_VERSION


#if PY_MAJOR_VERSION >= 3
# define PyInt_FromLong PyLong_FromLong
#endif

#define _cffi_from_c_double PyFloat_FromDouble
#define _cffi_from_c_float PyFloat_FromDouble
#define _cffi_from_c_long PyInt_FromLong
#define _cffi_from_c_ulong PyLong_FromUnsignedLong
#define _cffi_from_c_longlong PyLong_FromLongLong
#define _cffi_from_c_ulonglong PyLong_FromUnsignedLongLong

#define _cffi_to_c_double PyFloat_AsDouble
#define _cffi_to_c_float PyFloat_AsDouble

#define _cffi_from_c_int(x, type)                                        \
    (((type)-1) > 0 ? /* unsigned */                                     \
        (sizeof(type) < sizeof(long) ?                                   \
            PyInt_FromLong((long)x) :                                    \
         sizeof(type) == sizeof(long) ?                                  \
            PyLong_FromUnsignedLong((unsigned long)x) :                  \
            PyLong_FromUnsignedLongLong((unsigned long long)x)) :        \
        (sizeof(type) <= sizeof(long) ?                                  \
            PyInt_FromLong((long)x) :                                    \
            PyLong_FromLongLong((long long)x)))

#define _cffi_to_c_int(o, type)                                          \
    ((type)(                                                             \
     sizeof(type) == 1 ? (((type)-1) > 0 ? (type)_cffi_to_c_u8(o)        \
                                         : (type)_cffi_to_c_i8(o)) :     \
     sizeof(type) == 2 ? (((type)-1) > 0 ? (type)_cffi_to_c_u16(o)       \
                                         : (type)_cffi_to_c_i16(o)) :    \
     sizeof(type) == 4 ? (((type)-1) > 0 ? (type)_cffi_to_c_u32(o)       \
                                         : (type)_cffi_to_c_i32(o)) :    \
     sizeof(type) == 8 ? (((type)-1) > 0 ? (type)_cffi_to_c_u64(o)       \
                                         : (type)_cffi_to_c_i64(o)) :    \
     (Py_FatalError("unsupported size for type " #type), (type)0)))

#define _cffi_to_c_i8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[1])
#define _cffi_to_c_u8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[2])
#define _cffi_to_c_i16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[3])
#define _cffi_to_c_u16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[4])
#define _cffi_to_c_i32                                                   \
                 ((int(*)(PyObject *))_cffi_exports[5])
#define _cffi_to_c_u32                                                   \
                 ((unsigned int(*)(PyObject *))_cffi_exports[6])
#define _cffi_to_c_i64                                                   \
                 ((long long(*)(PyObject *))_cffi_exports[7])
#define _cffi_to_c_u64                                                   \
                 ((unsigned long long(*)(PyObject *))_cffi_exports[8])
#define _cffi_to_c_char                                                  \
                 ((int(*)(PyObject *))_cffi_exports[9])
#define _cffi_from_c_pointer                                             \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[10])
#define _cffi_to_c_pointer                                               \
    ((char *(*)(PyObject *, CTypeDescrObject *))_cffi_exports[11])
#define _cffi_get_struct_layout                                          \
    not used any more
#define _cffi_restore_errno                                              \
    ((void(*)(void))_cffi_exports[13])
#define _cffi_save_errno                                                 \
    ((void(*)(void))_cffi_exports[14])
#define _cffi_from_c_char                                                \
    ((PyObject *(*)(char))_cffi_exports[15])
#define _cffi_from_c_deref                                               \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[16])
#define _cffi_to_c                                                       \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[17])
#define _cffi_from_c_struct                                              \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[18])
#define _cffi_to_c_wchar_t                                               \
    ((wchar_t(*)(PyObject *))_cffi_exports[19])
#define _cffi_from_c_wchar_t                                             \
    ((PyObject *(*)(wchar_t))_cffi_exports[20])
#define _cffi_to_c_long_double                                           \
    ((long double(*)(PyObject *))_cffi_exports[21])
#define _cffi_to_c__Bool                                                 \
    ((_Bool(*)(PyObject *))_cffi_exports[22])
#define _cffi_prepare_pointer_call_argument                              \
    ((Py_ssize_t(*)(CTypeDescrObject *, PyObject *, char **))_cffi_exports[23])
#define _cffi_convert_array_from_object                                  \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[24])
#define _CFFI_CPIDX  25
#define _cffi_call_python                                                \
    ((void(*)(struct _cffi_externpy_s *, char *))_cffi_exports[_CFFI_CPIDX])
#define _CFFI_NUM_EXPORTS 26

typedef struct _ctypedescr CTypeDescrObject;

static void *_cffi_exports[_CFFI_NUM_EXPORTS];

#define _cffi_type(index)   (                           \
    assert((((uintptr_t)_cffi_types[index]) & 1) == 0), \
    (CTypeDescrObject *)_cffi_types[index])

static PyObject *_cffi_init(const char *module_name, Py_ssize_t version,
                            const struct _cffi_type_context_s *ctx)
{
    PyObject *module, *o_arg, *new_module;
    void *raw[] = {
        (void *)module_name,
        (void *)version,
        (void *)_cffi_exports,
        (void *)ctx,
    };

    module = PyImport_ImportModule("_cffi_backend");
    if (module == NULL)
        goto failure;

    o_arg = PyLong_FromVoidPtr((void *)raw);
    if (o_arg == NULL)
        goto failure;

    new_module = PyObject_CallMethod(
        module, (char *)"_init_cffi_1_0_external_module", (char *)"O", o_arg);

    Py_DECREF(o_arg);
    Py_DECREF(module);
    return new_module;

  failure:
    Py_XDECREF(module);
    return NULL;
}

_CFFI_UNUSED_FN
static PyObject **_cffi_unpack_args(PyObject *args_tuple, Py_ssize_t expected,
                                    const char *fnname)
{
    if (PyTuple_GET_SIZE(args_tuple) != expected) {
        PyErr_Format(PyExc_TypeError,
                     "%.150s() takes exactly %zd arguments (%zd given)",
                     fnname, expected, PyTuple_GET_SIZE(args_tuple));
        return NULL;
    }
    return &PyTuple_GET_ITEM(args_tuple, 0);   /* pointer to the first item,
                                                  the others follow */
}

/**********  end CPython-specific section  **********/
#else
_CFFI_UNUSED_FN
static void (*_cffi_call_python_org)(struct _cffi_externpy_s *, char *);
# define _cffi_call_python  _cffi_call_python_org
#endif


#define _cffi_array_len(array)   (sizeof(array) / sizeof((array)[0]))

#define _cffi_prim_int(size, sign)                                      \
    ((size) == 1 ? ((sign) ? _CFFI_PRIM_INT8  : _CFFI_PRIM_UINT8)  :    \
     (size) == 2 ? ((sign) ? _CFFI_PRIM_INT16 : _CFFI_PRIM_UINT16) :    \
     (size) == 4 ? ((sign) ? _CFFI_PRIM_INT32 : _CFFI_PRIM_UINT32) :    \
     (size) == 8 ? ((sign) ? _CFFI_PRIM_INT64 : _CFFI_PRIM_UINT64) :    \
     _CFFI__UNKNOWN_PRIM)

#define _cffi_prim_float(size)                                          \
    ((size) == sizeof(float) ? _CFFI_PRIM_FLOAT :                       \
     (size) == sizeof(double) ? _CFFI_PRIM_DOUBLE :                     \
     (size) == sizeof(long double) ? _CFFI__UNKNOWN_LONG_DOUBLE :       \
     _CFFI__UNKNOWN_FLOAT_PRIM)

#define _cffi_check_int(got, got_nonpos, expected)      \
    ((got_nonpos) == (expected <= 0) &&                 \
     (got) == (unsigned long long)expected)

#ifdef MS_WIN32
# define _cffi_stdcall  __stdcall
#else
# define _cffi_stdcall  /* nothing */
#endif

#ifdef __cplusplus
}
#endif

/************************************************************/

#include <string.h>
#include <stdio.h>
//#include "mmult_accel.h"

//#define debug(...) {printf(__VA_ARGS__);}
#define debug(...) {}

#define CACHE_SIZE (1024*16)

float x_row_cache[CACHE_SIZE];
float w_row_cache[CACHE_SIZE];
float y_col_cache[CACHE_SIZE];

static int r_xcnt = 0;
static int n_xread = 0;
static int n_xread_pix = 0;
static int r_xflg = 0;
float* x_get_next_line_offset(float* input, int width, int height) {
	int n_cache_line = CACHE_SIZE / width;

	if (n_cache_line > height) {
		n_cache_line = height;
	}

	if (width*height <= CACHE_SIZE) {
		if (r_xflg == 0) {
			debug("[%s] read from DRAM %d (1 time)\n", __func__, height*width);
			memcpy(x_row_cache, input, height * width * sizeof(float));
			r_xflg = 1;
		}
	} else {
		debug("[%s]r_xcnt:%d, cache_size: %d, width: %d, height: %d, n_cache_line:%d, n_xread:%d\n", __func__, r_xcnt, CACHE_SIZE, width, height, n_cache_line, n_xread);
		if (r_xcnt == 0) {
			debug("[%s]read from DRAM %d\n", __func__, n_xread*n_cache_line*width);
			memcpy(x_row_cache, &input[n_xread * n_cache_line * width], n_cache_line * width * sizeof(float));
			n_xread++;
			n_xread_pix += n_cache_line * width;
			if (n_xread_pix == width*height) {
			    debug("[%s] read all pixels\n", __func__);
				n_xread = 0;
				n_xread_pix = 0;
			}
		}
	}

	int idx = r_xcnt;
	if (r_xcnt == n_cache_line - 1) {
		r_xcnt = 0;
	} else {
		r_xcnt++;
	}
	debug("[%s] offset:%d\n", __func__, idx*width);
	return &x_row_cache[idx * width];
}

static int r_wcnt = 0;
static int n_wread = 0;
static int n_wread_pix = 0;
static int r_wflg = 0;
float* w_get_next_line_offset(float* input, int width, int height) {
	int n_cache_line = CACHE_SIZE / width;

	if (n_cache_line > height) {
		n_cache_line = height;
	}

	if (width*height <= CACHE_SIZE) {
		if (r_wflg == 0) {
			debug("[%s] read from DRAM %d (1 time)\n", __func__, height*width);
			memcpy(w_row_cache, input, height * width * sizeof(float));
			r_wflg = 1;
		}
	} else {
		debug("[%s]r_wcnt:%d, cache_size: %d, width: %d, height: %d, n_cache_line:%d, n_wread:%d\n", __func__, r_wcnt, CACHE_SIZE, width, height, n_cache_line, n_wread);
		if (r_wcnt == 0) {
			memcpy(w_row_cache, &input[n_wread * n_cache_line * width], n_cache_line * width * sizeof(float));
			n_wread++;
			n_wread_pix += n_cache_line * width;
			if (n_wread_pix == width*height) {
				n_wread = 0;
				n_wread_pix = 0;
			}
		}
	}

	int idx = r_wcnt;
	if (r_wcnt == n_cache_line - 1) {
		r_wcnt = 0;
	} else {
		r_wcnt++;
	}
	debug("[%s] offset:%d\n", __func__, idx*width);
	return &w_row_cache[idx * width];
}

static int w_cnt = 0;
static int n_write = 0;
static int n_write_pix = 0;
void y_write_cahce(float* output, int width, int height, float data) {
	int n_cache_line = CACHE_SIZE / width;

	if (n_cache_line > height) {
		n_cache_line = height;
	}

	debug("[%s] w_cnt:%d, width:%d, height:%d, n_cache_line:%d, n_write:%d, data:%f\n", __func__, w_cnt, width, height, n_cache_line, n_write, data);
	y_col_cache[w_cnt] = data;

	// if cache is full then write to DRAM.
	if (w_cnt == width*n_cache_line - 1) {
		// write n lines
		debug("[%s] write to DRAM offset:%d. size:%d\n",__func__ , n_write*width, width);
		memcpy(&output[n_write*width*n_cache_line], y_col_cache, width * n_cache_line * sizeof(float));
		n_write++;
		n_write_pix += n_cache_line * width;
		if (n_write_pix == width*height) {
			n_write = 0;
			n_write_pix = 0;
		}
		w_cnt = 0;
	} else {
		w_cnt++;
	}
}

#pragma SDS data sys_port(x:ACP, w:ACP, y:ACP)
#pragma SDS data access_pattern(x:SEQUENTIAL)
#pragma SDS data access_pattern(w:SEQUENTIAL)
#pragma SDS data access_pattern(y:SEQUENTIAL)
#pragma SDS data mem_attribute(x:PHYSICAL_CONTIGUOUS, w:PHYSICAL_CONTIGUOUS, y:PHYSICAL_CONTIGUOUS)
#pragma SDS data zero_copy(x[0:CACHE_SIZE])
#pragma SDS data zero_copy(w[0:CACHE_SIZE])
#pragma SDS data zero_copy(y[0:CACHE_SIZE])
int mmult_accel1(float *x, float *w, float *y, int x_nrows, int w_nrows, int xw_ncols) {
	float *x_row_cache_;
	float *w_row_cache_;
	
	debug("[%s] x: (%d, %d), w: (%d, %d) => y: (%d, %d)\n",__func__ , x_nrows, xw_ncols, w_nrows, xw_ncols, x_nrows, w_nrows);
	r_xcnt = 0;
	n_xread = 0;
	n_xread_pix = 0;
	r_wcnt = 0;
	n_wread = 0;
	n_wread_pix = 0;
	w_cnt = 0;
	n_write = 0;
	n_write_pix = 0;
	r_xflg = 0;
	r_wflg = 0;

	for (int w_row = 0; w_row < w_nrows; w_row++) {

		// read 1 row from DRAM
		debug("[%s] i:%d\n", __func__, w_row);
		w_row_cache_ = w_get_next_line_offset(w, xw_ncols, w_nrows);
		//memcpy(w_row_cache, &w[w_row*xw_ncols], xw_ncols * sizeof(float));

		for (int x_row = 0; x_row < x_nrows; x_row++) {
#pragma HLS PIPELINE II=1
			// read 1 col from DRAM
			debug("[%s] i:%d, j:%d\n", __func__, w_row, x_row);
			x_row_cache_ = x_get_next_line_offset(x, xw_ncols, x_nrows);
			//memcpy(x_row_cache, &x[x_row*xw_ncols], xw_ncols * sizeof(float));

			float result = 0.0;
			for (int k = 0; k < xw_ncols; k++) {
				//debug("[%s] %d x:%f, w:%f\n", __func__, k, x_row_cache_[k], w_row_cache_[k]);
				result += x_row_cache_[k] * w_row_cache_[k];
				//result += in_A[row * a_ncols + k] * in_B[k * b_ncols + col];
			}
			//y_col_cache[x_row] = result;
			debug("[%s] result:%f\n", __func__, result);
			y_write_cahce(y, w_nrows, x_nrows, result);
			//y_write_cahce(y, x_nrows, w_nrows, result);
			////y_write_cahce(y, xw_ncols, x_nrows, result);
		}
		// write 1 col to DRAM
		//memcpy(&y[w_row*xw_ncols], y_col_cache, xw_ncols * sizeof(float));
		//memcpy(&y[w_row*x_nrows], y_col_cache, x_nrows * sizeof(float));
	}
	return 0;
}


/************************************************************/

static void *_cffi_types[] = {
/*  0 */ _CFFI_OP(_CFFI_OP_FUNCTION, 4), // int()(float *, float *, float *, int, int, int)
/*  1 */ _CFFI_OP(_CFFI_OP_POINTER, 8), // float *
/*  2 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/*  3 */ _CFFI_OP(_CFFI_OP_NOOP, 1),
/*  4 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7), // int
/*  5 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/*  6 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/*  7 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  8 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 13), // float
};

static int _cffi_d_mmult_accel1(float * x0, float * x1, float * x2, int x3, int x4, int x5)
{
  return mmult_accel1(x0, x1, x2, x3, x4, x5);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_mmult_accel1(PyObject *self, PyObject *args)
{
  float * x0;
  float * x1;
  float * x2;
  int x3;
  int x4;
  int x5;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 6, "mmult_accel1");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];
  arg5 = aa[5];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (float *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(1), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (float *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(1), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(1), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (float *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(1), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_int(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_int(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = mmult_accel1(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_mmult_accel1 _cffi_d_mmult_accel1
#endif

static const struct _cffi_global_s _cffi_globals[] = {
  { "mmult_accel1", (void *)_cffi_f_mmult_accel1, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 0), (void *)_cffi_d_mmult_accel1 },
};

static const struct _cffi_type_context_s _cffi_type_context = {
  _cffi_types,
  _cffi_globals,
  NULL,  /* no fields */
  NULL,  /* no struct_unions */
  NULL,  /* no enums */
  NULL,  /* no typenames */
  1,  /* num_globals */
  0,  /* num_struct_unions */
  0,  /* num_enums */
  0,  /* num_typenames */
  NULL,  /* no includes */
  9,  /* num_types */
  0,  /* flags */
};

#ifdef PYPY_VERSION
PyMODINIT_FUNC
_cffi_pypyinit_overlay(const void *p[])
{
    p[0] = (const void *)0x2601;
    p[1] = &_cffi_type_context;
}
#  ifdef _MSC_VER
     PyMODINIT_FUNC
#  if PY_MAJOR_VERSION >= 3
     PyInit_overlay(void) { return NULL; }
#  else
     initoverlay(void) { }
#  endif
#  endif
#elif PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit_overlay(void)
{
  return _cffi_init("overlay", 0x2601, &_cffi_type_context);
}
#else
PyMODINIT_FUNC
initoverlay(void)
{
  _cffi_init("overlay", 0x2601, &_cffi_type_context);
}
#endif
