#include <stdlib.h>
#include <iconv.h>
#include <langinfo.h>
#include <locale.h>
#include <errno.h>

static char *iconv_from_utf8_to_locale(char *string, char* fallback_string)
{
  const char *charset;

  iconv_t cd;
  size_t nconv;

  char *buffer;
  char *buffer_ptr;
  char *string_ptr;

  const size_t buffer_inc = 80;	// Increment buffer size in 80 bytes step
  size_t buffer_size;
  size_t buffer_size_left;
  size_t string_size;

  // Get the current charset
  setlocale(LC_ALL, "");
  charset = nl_langinfo (CODESET);

  // Open iconv descriptor
  cd = iconv_open(charset, "UTF-8");
  if (cd == (iconv_t) -1)
     return fallback_string;

  // Set up the buffer
  buffer_size = buffer_size_left = buffer_inc;
  buffer = (char *) malloc(buffer_size + 1);
  if (buffer == NULL)
    return fallback_string;
  buffer_ptr = buffer;
  string_ptr = string;
  string_size = strlen(string);
  
  // Do the conversion
  while (string_size != 0)
  {
    nconv = iconv(cd, &string_ptr, &string_size, &buffer_ptr, &buffer_size_left);
    if (nconv == (size_t) -1)
    {
      if (errno != E2BIG)   	// if translation error
      {
        iconv_close(cd);	// close descriptor
        free(buffer);  		// free buffer
        return fallback_string; // and return fallback string
      }
      // increase buffer size
      buffer_size += buffer_inc;
      buffer_size_left = buffer_inc;
      buffer = (char *) realloc(buffer, buffer_size + 1);
      if (buffer == NULL)
        return fallback_string;
    }
  }
  *buffer_ptr = '\0';
  iconv_close(cd);
  buffer = (char *) realloc(buffer, buffer_size + 1);
  return buffer;
}

char *degree_sign()
{
  unsigned char str[] = { 0xc2, 0xb0, 0x00 };

  return iconv_from_utf8_to_locale(str, " \0");
}

