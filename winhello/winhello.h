#ifndef WINHELLO_H
#define WINHELLO_H

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) int winhello(void);
__declspec(dllexport) int winhello2(void);

#ifdef __cplusplus
}
#endif

#endif /* WINHELLO_H */
