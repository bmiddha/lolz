#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <wchar.h>
#include <stdarg.h>

#define ARRAY_SIZE(foo) (sizeof(foo) / sizeof(foo[0]))
const unsigned char codes[] = {39, 38, 44, 43, 49, 48, 84, 83, 119, 118, 154, 148, 184, 178, 214, 208, 209, 203, 204, 198, 199, 163, 164, 128, 129, 93, 99, 63, 69, 33};

static void find_escape_sequences(wint_t c, int *state)
{
    if (c == '\033')
    {
        *state = 1;
    }
    else if (*state == 1)
    {
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
            *state = 2;
    }
    else
    {
        *state = 0;
    }
}

void lolcat(char *input)
{
    int cc = -1, i, l = 0;
    int colors = isatty(STDOUT_FILENO);
    double freq_h = 0.23, freq_v = 0.1;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    double offx = (tv.tv_sec % 300) / 300.0;

    i = 0;
    int escape_state = 0;

    size_t j = 0;
    while (j < strlen(input))
    {
        wchar_t c = input[j];
        if (colors)
        {
            find_escape_sequences(c, &escape_state);

            if (!escape_state)
            {
                if (c == '\n')
                {
                    l++;
                    i = 0;
                }
                else
                {
                    int ncc = offx * ARRAY_SIZE(codes) + (int)((i += wcwidth(c)) * freq_h + l * freq_v);
                    if (cc != ncc)
                        wprintf(L"\033[38;5;%hhum", codes[(cc = ncc) % ARRAY_SIZE(codes)]);
                }
            }
        }

        putwchar(c);

        if (escape_state == 2)
            wprintf(L"\033[38;5;%hhum", codes[cc % ARRAY_SIZE(codes)]);
        j++;
    }

    if (colors)
        wprintf(L"\033[0m");

    cc = -1;
}


void lolz (char *fmt, ...) {
    char *buf = malloc (sizeof(fmt));
    va_list va;
    va_start (va, fmt);
    vsprintf (buf, fmt, va);
    lolcat(buf);
    va_end (va);
    free(buf);
}
