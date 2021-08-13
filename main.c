/**********************************************************************

  main.c -

  $Author$
  created at: Fri Aug 19 13:19:58 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

/*!
 * \mainpage Developers' documentation for Ruby
 *
 * This documentation is produced by applying Doxygen to
 * <a href="https://github.com/ruby/ruby">Ruby's source code</a>.
 * It is still under construction (and even not well-maintained).
 * If you are familiar with Ruby's source code, please improve the doc.
 */
#undef RUBY_EXPORT
#include "ruby.h"
#include "vm_debug.h"
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#if defined RUBY_DEVEL && !defined RUBY_DEBUG_ENV
# define RUBY_DEBUG_ENV 1
#endif
#if defined RUBY_DEBUG_ENV && !RUBY_DEBUG_ENV
# undef RUBY_DEBUG_ENV
#endif
static void sandbox(void);
static int
rb_main(int argc, char **argv)
{
    RUBY_INIT_STACK;
    ruby_init(); // sandbox();
    return ruby_run_node(ruby_options(argc, argv));
}

#if defined(__wasm__) && !defined(__EMSCRIPTEN__)
int rb_wasm_rt_start(int (main)(int argc, char **argv), int argc, char **argv);
#define rb_main(argc, argv) rb_wasm_rt_start(rb_main, argc, argv)
#endif

int
main(int argc, char **argv)
{
#ifdef RUBY_DEBUG_ENV
    ruby_set_debug_option(getenv("RUBY_DEBUG"));
#endif
#ifdef HAVE_LOCALE_H
    setlocale(LC_CTYPE, "");
#endif

    ruby_sysinit(&argc, &argv);
    return rb_main(argc, argv);
}

static void sandbox_0_cdefs(void);
static void sandbox_1_constants(void);
static void sandbox_1_types(void);
static void sandbox_object(void);
static void sandbox_string(void);
static void sandbox_variable(void);

static void sandbox(void) {
    sandbox_0_cdefs();
    sandbox_1_constants();
    sandbox_1_types();
    sandbox_object();
    sandbox_string();
    sandbox_variable();
    __asm__("nop");
}

static void sandbox_0_cdefs(void) {
    if (RB_LIKELY(TRUE)) {}
    if (RB_UNLIKELY(FALSE)) {}

    RBIMPL_CAST((size_t)42U); // g++ のキャストのバグ対策らしい; Cでは何もしないのでruby内部では使う必要な無い

    return;
    UNREACHABLE;
}

static void sandbox_1_constants(void) {
    VALUE obj;
    bool b;

    obj = rb_eval_string("nil"); // == RUBY_Qnil

    (void)RUBY_Qfalse;         // = 0x00, /* ...0000 0000 */
    (void)RUBY_Qnil;           // = 0x04, /* ...0000 0100 */
    (void)RUBY_Qtrue;          // = 0x14, /* ...0001 0100 */
    (void)RUBY_Qundef;         // = 0x24, /* ...0010 0100 */

    (void)RUBY_IMMEDIATE_MASK; // = 0x07, /* ...0000 0111 */
    // ... x111 のどれか1つでも 1 なら即値
    // xxx1 FIXNUM
    // xx10 FLONUM
    // 0100 nil true undef 他は何かある？
    // 1100 SYMBOL
    // VALUE が special constant でない場合はポインタで、アドレスは8バイトalign 0x0000 0x0008 0x00a0 0x00a8 になっている事が前提のようだ
    //
    (void)(bool)(RUBY_Qnil & RUBY_IMMEDIATE_MASK); // true
    b = RB_IMMEDIATE_P(RUBY_Qnil); // true

    (void)RUBY_FIXNUM_FLAG;    // = 0x01, /* ...xxxx xxx1 */
    obj = rb_eval_string("-3"); // 0xfffffffffffffffb ...1111 1011 2の補数
    obj = rb_eval_string("-2"); // 0xfffffffffffffffd ...1111 1101
    obj = rb_eval_string("-1"); // 0xffffffffffffffff ...1111 1111
    obj = rb_eval_string("0"); // 1 ...0000 0001
    obj = rb_eval_string("1"); // 3 ...0000 0011
    obj = rb_eval_string("2"); // 5 ...0000 0101
    (void)(bool)(obj & RUBY_FIXNUM_FLAG); // true
    RB_FIXNUM_P(obj); // true

    (void)RUBY_FLONUM_MASK;    // = 0x03, /* ...0000 0011 */
    (void)RUBY_FLONUM_FLAG;    // = 0x02, /* ...xxxx xx10 */
    obj = rb_eval_string("0.0"); // 0x8000000000000002 ... 0010
    obj = rb_eval_string("0.1"); // 0xfdccccccccccccd2 ... 0010
    obj = rb_eval_string("1.0"); // 0xff80000000000002 ... 0010
    obj = rb_eval_string("-1.0"); // 0xff80000000000006 ... 0110

    (void)RUBY_SYMBOL_FLAG;    // = 0x0c, /* ...xxxx 1100 */
    obj = rb_eval_string(":foo"); // 0x76110c ... 1100
    obj = rb_eval_string(":bar"); // 0x76210c ... 1100
    RB_STATIC_SYM_P(obj); // true

    // RUBY_Qfalse が 0000 で例外的なので RB_SPECIAL_CONST_P() で false も拾えるようになっている
    // RB_IMMEDIATE_P(obj) || obj == RUBY_Qfalse;
    RB_SPECIAL_CONST_P(RUBY_Qfalse); // true

    __asm__("nop");
}

static void sandbox_1_types(void) {
    // https://docs.ruby-lang.org/ja/latest/library/_builtin.html

    (void)rb_mKernel;                 /**< `Kernel` module. */ // object.c : rb_mKernel = rb_define_module("Kernel");
    (void)rb_mComparable;             /**< `Comparable` module. */ // compar.c : rb_mComparable = rb_define_module("Comparable");
    (void)rb_mEnumerable;             /**< `Enumerable` module. */ // enum.c : rb_mEnumerable = rb_define_module("Enumerable");
    (void)rb_mErrno;                  /**< `Errno` module. */ // error.c : rb_mErrno = rb_define_module("Errno");
    (void)rb_mFileTest;               /**< `FileTest` module. */ // file.c : rb_mFileTest = rb_define_module("FileTest");
    (void)rb_mGC;                     /**< `GC` module. */ // gc.c : rb_mGC = rb_define_module("GC");
    (void)rb_mMath;                   /**< `Math` module. */ // math.c : rb_mMath = rb_define_module("Math");
    (void)rb_mProcess;                /**< `Process` module. */ // process.c : rb_mProcess = rb_define_module("Process");
    (void)rb_mWaitReadable;           /**< `IO::WaitReadable` module. */ // io.c : rb_mWaitReadable = rb_define_module_under(rb_cIO, "WaitReadable");
    (void)rb_mWaitWritable;           /**< `IO::WaitReadable` module. */ // io.c : rb_mWaitWritable = rb_define_module_under(rb_cIO, "WaitWritable");

    // https://docs.ruby-lang.org/ja/latest/class/Array.html
    // Array < Enumerable < Object < Kernel < BasicObject

    (void)rb_cBasicObject;            /**< `BasicObject` class. */ // class.c : rb_cBasicObject = boot_defclass("BasicObject", 0);  object.c : rb_cBasicObject = rb_define_class("BasicObject", Qnil);
    (void)rb_cObject;                 /**< `Object` class. */ // class.c : rb_cObject = boot_defclass("Object", rb_cBasicObject);
    (void)rb_cArray;                  /**< `Array` class. */ // array.c : rb_cArray  = rb_define_class("Array", rb_cObject);
    (void)rb_cBinding;                /**< `Binding` class. */ // proc.c : rb_cBinding = rb_define_class("Binding", rb_cObject);
    (void)rb_cClass;                  /**< `Class` class. */ // class.c : rb_cClass =  boot_defclass("Class",  rb_cModule);
    (void)rb_cDir;                    /**< `Dir` class. */
    (void)rb_cEncoding;               /**< `Encoding` class. */
    (void)rb_cEnumerator;             /**< `Enumerator` class. */
    (void)rb_cFalseClass;             /**< `FalseClass` class. */
    (void)rb_cFile;                   /**< `File` class. */
    (void)rb_cComplex;                /**< `Complex` class. */
    (void)rb_cFloat;                  /**< `Float` class. */
    (void)rb_cHash;                   /**< `Hash` class. */
    (void)rb_cIO;                     /**< `IO` class. */
    (void)rb_cInteger;                /**< `Module` class. */
    (void)rb_cMatch;                  /**< `MatchData` class. */
    (void)rb_cMethod;                 /**< `Method` class. */
    (void)rb_cModule;                 /**< `Module` class. */
    (void)rb_cRefinement;             /**< `Refinement` class. */
    (void)rb_cNameErrorMesg;          /**< `NameError::Message` class. */
    (void)rb_cNilClass;               /**< `NilClass` class. */
    (void)rb_cNumeric;                /**< `Numeric` class. */
    (void)rb_cProc;                   /**< `Proc` class. */
    (void)rb_cRandom;                 /**< `Random` class. */
    (void)rb_cRange;                  /**< `Range` class. */
    (void)rb_cRational;               /**< `Rational` class. */
    (void)rb_cRegexp;                 /**< `Regexp` class. */
    (void)rb_cStat;                   /**< `File::Stat` class. */
    (void)rb_cString;                 /**< `String` class. */
    (void)rb_cStruct;                 /**< `Struct` class. */
    (void)rb_cSymbol;                 /**< `Sumbol` class. */
    (void)rb_cThread;                 /**< `Thread` class. */
    (void)rb_cTime;                   /**< `Time` class. */
    (void)rb_cTrueClass;              /**< `TrueClass` class. */
    (void)rb_cUnboundMethod;          /**< `UnboundMethod` class. */

    const struct RBasic *b = RBASIC(rb_mKernel); // cast VALUE -> RBasic

    (void)((RBASIC(rb_mKernel)->flags & RUBY_T_MASK) == RUBY_T_MODULE); // 0xd00000043 & 0x1f
    enum ruby_value_type t = RB_BUILTIN_TYPE(rb_mKernel); // RUBY_T_MODULE

    (void)rb_integer_type_p; // RB_FIXNUM_P() or RUBY_T_BIGNUM

    // RB_BUILTIN_TYPE() の上位互換で、special constant でも使える（SEGVしない）
    rb_type(RUBY_Qnil); // RUBY_T_NIL

    (void)RB_FLOAT_TYPE_P;
    (void)RB_DYNAMIC_SYM_P;
    (void)RB_SYMBOL_P;

    (void)(bool)RB_TYPE_P(RUBY_Qnil, RUBY_T_NIL); // true
    Check_Type(RUBY_Qnil, RUBY_T_NIL); // ok
    // Check_Type(RUBY_Qnil, RUBY_T_TRUE); // die

    __asm__("nop");
}

static void sandbox_object(void) {
    VALUE obj = rb_obj_alloc(rb_cArray); // == Array.new ?
    __asm__("nop");
}

static void sandbox_string(void) {
    // StringValueCStr(str);
    // rb_string_value_cstr
    __asm__("nop");
}

static void sandbox_variable(void) {
    (void)rb_iv_get; // instance variable get
    (void)rb_iv_set; // instance variable set
    __asm__("nop");
}
