# Macro-Controlled Logging (MCL) API

MCL is a lightweight logging API for C/C++ based on the C
preprocessor.  It is designed to provide an API inspired by log4j-like
APIs, but with minimal overhead for the application.  Use of the
preprocessor allows logging statements to be hardwired in place,
removed at compile-time, or dynamically enabled and disabled at
runtime.