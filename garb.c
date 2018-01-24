/*
  Copyright (c) 2005-2018 Chris Cuthbertson

  This file is part of garb.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void parsefile(FILE **fp, char *buf, int sz);

#define FILE_STUBTEXT "stubtext.conf"
#define FILE_MESSAGE "message.conf"

#define OFFSET 0x40

void main(int argc, char **argv)
{
  FILE *fp;
  int wr, i, j, len;
  char *buf;
  char *fr = NULL;
  char message[2048];
  char stubtext[512];

  if (argc != 2) {
    printf("syntax: [file]\n");
    return;
  }

  //** STUBTEXT **
  memset(stubtext, '\0', sizeof(stubtext));
  fp = fopen(FILE_STUBTEXT, "rt");
  if (!fp) {
    printf("couldn\'t open " FILE_STUBTEXT "\n");
    goto _end;
  }
  parsefile(&fp, stubtext, sizeof(stubtext));
  fclose(fp);

  //** MESSAGE **
  memset(message, '\0', sizeof(message));
  fp = fopen(FILE_MESSAGE, "rt");
  if (!fp) {
    printf("couldn\'t open " FILE_MESSAGE "\n");
    return;
  }
  parsefile(&fp, message, sizeof(message));
  fclose(fp);

  //** EXE EDIT **
  fp = fopen(argv[1], "rb");
  if (!fp) {
    printf("couldn\'t open %s\n", argv[1]);
    return;
  }

  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  buf = (char *)malloc(len+1);
  if (!buf) {
    printf("couldn\'t alloc memory\n");
    return;
  }
  fr = buf;

  fread(buf, 1, len, fp);
  fclose(fp);

  fp = fopen(argv[1], "wb");
  if (!fp) {
    printf("couldn\'t open %s\n", argv[1]);
    goto _end;
  }

  wr = 0;
  i = strlen(stubtext);
  wr += fwrite(buf, 1, OFFSET, fp);
  wr += fwrite(stubtext, 1, i, fp);
  buf += OFFSET + i;
  j = strlen(message);
  wr += fwrite(buf, 1, len - OFFSET - i - j - 1, fp);
  wr += fwrite(message, 1, j, fp);
  buf[0] = '\0';
  wr += fwrite(buf, 1, 1, fp);

  printf("%s: %d written\n", argv[1], wr);

  fclose(fp);

_end:
  if (fr)
    free(fr);

  return;
}

void parsefile(FILE **fp, char *buf, int sz)
{
  char c;
  int i, j;
  char *s, *p, *q;

  p = malloc(sz + 1);
  if (!p)
    return;

  i = fread(p, 1, sz, *fp);
  p[i] = '\0';

  j = 0;
  s = p;
  q = buf;
  while (*s)
  {
    if (*s == '\\' && s[1] == 'x')
    {
      c = tolower(s[2]);
      if (c >= '0' && c <= '9')
        i = (c - '0') << 4;
      else if (c >= 'a' && c <= 'f')
        i = (c - 'a' + 10) << 4;
      else goto _skip;
      c = tolower(s[3]);
      if (c >= '0' && c <= '9')
        i += (c - '0');
      else if (c >= 'a' && c <= 'f')
        i += (c - 'a' + 10);
      else
        goto _skip;
      *q++ = i;
      j++;
      if (j >= sz)
        break;
      s += 4;
      continue;
    }
_skip:
    if (*s != '\n' && *s != '\r' && *s != '\t')
      *q++ = *s;
    s++;
    j++;
    if (j >= sz)
      break;
  }
  *q = '\0';

  free(p);
}
