//
// Created by karol on 26.10.24.
//

#ifndef ERROR_UTILITES_H
#define ERROR_UTILITES_H

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression)             \
(__extension__({                               \
long int __result;                         \
do                                         \
__result = (long int)(expression);     \
while (__result == -1L && errno == EINTR); \
__result;                                  \
}))
#endif

#define ERR_AND_EXIT(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

#define REPORT_ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__))

#define INTERNAL_ERROR(reason) (fprintf(stderr, "%s: at %s:%d\n", reason, __FILE__, __LINE__))

#endif //ERROR_UTILITES_H
