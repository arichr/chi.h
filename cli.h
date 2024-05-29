/**
 * MIT License
 *
 * Copyright (c) 2024 Arisu Wonderland
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// A small stb-style library for handling the command line.
//
// To use a provided implementation, define CLI_IMPLEMENTATION before including
// the file:
//     #define CLI_IMPLEMENTATION
//     #include "cli.h"
//
// The behaviour and dependencies of this library can be configured with defining
// these macros:
//     CLI_NO_STDIO_H
//         Do not include <stdio.h> (but fprintf() is still expected to be
//         defined).
//     CLI_NO_STDBOOL_H
//         Do not use <stdbool.h>.
//     CLI_NO_STYLES
//         Do not use colors and other styles for formatting output.
//
//     CLI_DEFAULT_ARR_CAP = 5
//         Default capacity for dynamic arrays (e.g. CliStringArray).
//     CLI_ASSERT = assert
//         An assert function. If not defined, assert() form <assert.h> is
//         used.
//     CLI_MALLOC = malloc
//         A function for allocating memory. If not defined, malloc() from
//         <stdlib.h> is used.
//     CLI_FREE = free
//         A function for freeing memory. If not defined, free() from
//         <stdlib.h> is used.
//     CLI_ERROR_SYM = "✖"
//         A symbol to use in error messages. By default, it is a Unicode
//         'x' that may be unsupported by a terminal font.
//     CLI_INFO_SYM = "●"
//         A symbol to use in information messages. By default, it is a Unicode
//         circle that may be unsupported by a terminal font.
//
// Like CLI_IMPLEMENTATION, all defines that how cli.h is processed should
// be placed before the #include directive.
// Order of #define directives are not known to cli.h, thus is not important.
//
// Example pseudo-code:
//
// int main(int argc, char** argv) {
//     Cli cli;
//
//     int exit_code = cli_parse(argc, argv, &cli);
//     if (exit_code) {
//         return exit_code;
//     }
//     process_options(&cli);
//     if (--color == "yes") {
//         cli_toggle_colors();
//     }
//     // ...
// }
//

#ifndef __CLI_H_
#define __CLI_H_

#ifdef __cplusplus
extern "C" {
#endif

const char* CLI_RESET = "";
const char* CLI_BOLD = "";
const char* CLI_DIM = "";
const char* CLI_FORE_RED = "";
const char* CLI_FORE_BRBLUE = "";

struct CliStringArray {
	unsigned short length;
	unsigned short capacity;
	const char** data;
};

typedef struct Cli {
	const char* execfile;
	const char* command;
	struct CliStringArray args;
	struct CliStringArray cmd_options;
	struct CliStringArray program_options;
} Cli;

enum CliError {
	CliErrorOk,
	CliErrorUser,
	CliErrorFatal
};

/* Initialize variables for formatting output.
 *
 * If `CLI_RESET` contains an empty string, all variables are initalized with
 * escape sequences, according to their names.
 * Otherwise, variables are initalized with empty strings.
 */
void cli_toggle_colors(void);

enum CliError cli_parse(int argc, char** argv, Cli* cli);

// Free memory occupied by dynamic arrays.
inline void cli_free(Cli* cli);

/********************************** =(^-x-^)= **********************************/

#ifdef CLI_IMPLEMENTATION

#ifdef CLI_NO_STDBOOL_H
#define bool  unsigned short
#define true  1
#define false 0
#else
#include <stdbool.h>
#endif // CLI_NO_STDBOOL_H

#ifndef CLI_NO_STDIO_H
#include <stdio.h>
#endif

#if !defined CLI_MALLOC || !defined CLI_FREE
#include <stdlib.h>
#endif

#ifndef CLI_MALLOC
#define CLI_MALLOC malloc
#endif

#ifndef CLI_FREE
#define CLI_FREE free
#endif

#ifndef CLI_ASSERT
#include <assert.h>
#define CLI_ASSERT assert
#endif

#ifndef CLI_DEFAULT_ARR_CAP
#define CLI_DEFAULT_ARR_CAP 5
#endif

#define CLI_STR_(x) #x
#define CLI_STR(x)	CLI_STR_(x)

#ifndef CLI_ERROR_SYM
#define CLI_ERROR_SYM "✖"
#endif

#ifndef CLI_INFO_SYM
#define CLI_INFO_SYM "●"
#endif

#define cli_da_init(array, item_t, da_malloc)                             \
	{                                                                     \
		(array).capacity = CLI_DEFAULT_ARR_CAP;                           \
		(array).length = 0;                                               \
		(array).data = (da_malloc)(CLI_DEFAULT_ARR_CAP * sizeof(item_t)); \
	}

#define cli_da_append(array, item)                                        \
	{                                                                     \
		if (++(array).length > (array).capacity) {                        \
			assert(0 && "TODO: Allocate an array with bigger capacity."); \
		}                                                                 \
		(array).data[(array).length - 1] = (item);                        \
	}

#define cli_print_error(title, msg)                                                        \
	fprintf(                                                                               \
		stderr, "%s" CLI_ERROR_SYM "%s%s " title "%s: " msg "\n", CLI_FORE_RED, CLI_RESET, \
		CLI_BOLD, CLI_RESET                                                                \
	)
#define cli_printf_error(title, msg, ...)                                                  \
	fprintf(                                                                               \
		stderr, "%s" CLI_ERROR_SYM "%s%s " title "%s: " msg "\n", CLI_FORE_RED, CLI_RESET, \
		CLI_BOLD, CLI_RESET, __VA_ARGS__                                                   \
	)
#define cli_print_info(title, msg)                                                           \
	fprintf(                                                                                 \
		stderr, "%s" CLI_INFO_SYM "%s%s " title "%s: " msg "\n", CLI_FORE_BRBLUE, CLI_RESET, \
		CLI_BOLD, CLI_RESET                                                                  \
	)
#define cli_printf_info(title, msg, ...)                                                     \
	fprintf(                                                                                 \
		stderr, "%s" CLI_INFO_SYM "%s%s " title "%s: " msg "\n", CLI_FORE_BRBLUE, CLI_RESET, \
		CLI_BOLD, CLI_RESET, __VA_ARGS__                                                     \
	)
#define cli_printf_debug(msg, ...)                                                                 \
	fprintf(                                                                                       \
		stderr, "%s" __FILE__ ":" CLI_STR(__LINE__) ":%s%sDebug%s: " msg "\n", CLI_DIM, CLI_RESET, \
		CLI_BOLD, CLI_RESET, __VA_ARGS__                                                           \
	)

void cli_toggle_colors(void) {
#ifndef CLI_NO_STYLES
	if (CLI_RESET[0]) {
		CLI_RESET = "";
		CLI_BOLD = "";
		CLI_DIM = "";
		CLI_FORE_RED = "";
		CLI_FORE_BRBLUE = "";
	} else {
		CLI_RESET = "\033[0m";
		CLI_BOLD = "\033[1m";
		CLI_DIM = "\033[2m";
		CLI_FORE_RED = "\033[31m";
		CLI_FORE_BRBLUE = "\033[94m";
	}
#endif // CLI_NO_STYLES
}

static const char* cli_pop_argv(int* argc, char*** argv) {
	CLI_ASSERT(*argc);
	(*argc)--;
	return *((*argv)++);
}

enum CliError cli_parse(int argc, char** argv, Cli* cli) {
	const char* execfile = cli_pop_argv(&argc, &argv);
	if (argc > 0) {
		cli->execfile = execfile;
		cli_da_init(cli->args, const char*, CLI_MALLOC);
		if (cli->args.data == NULL) {
			cli_print_error("Memory error", "Unable to allocate memory for CLI arguments.");
			return CliErrorFatal;
		}
		cli_da_init(cli->cmd_options, const char*, CLI_MALLOC);
		if (cli->cmd_options.data == NULL) {
			cli_print_error("Memory error", "Unable to allocate memory for command options.");
			return CliErrorFatal;
		}
		cli_da_init(cli->program_options, const char*, CLI_MALLOC);
		if (cli->program_options.data == NULL) {
			cli_print_error("Memory error", "Unable to allocate memory for program options.");
			return CliErrorFatal;
		}

		const char* arg;
		bool is_cmd_option = false;
		while (argc) {
			arg = cli_pop_argv(&argc, &argv);
			if (arg[0] == '-') {
				struct CliStringArray* option_array
					= is_cmd_option ? &cli->cmd_options : &cli->program_options;

				if (arg[1] == '-' && arg[2] == '\0') {
					if (cli->args.length > 0) {
						cli_printf_error(
							"CLI error",
							"Double dash ('%s') cannot be specified after the positional argument "
							"('%s').",
							arg, cli->args.data[cli->args.length - 1]
						);
						return CliErrorUser;
					}
					is_cmd_option = true;
				} else {
					cli_da_append(*option_array, arg);
				}
			} else {
				cli_da_append(cli->args, arg);
			}
		}
	} else {
		*cli = (struct Cli) { 0 };
		cli->execfile = execfile;
	}
	return CliErrorOk;
}

inline void cli_free(Cli* cli) {
	CLI_FREE(cli->args.data);
	CLI_FREE(cli->cmd_options.data);
	CLI_FREE(cli->program_options.data);
}

#endif // CLI_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // __CLI_H_
