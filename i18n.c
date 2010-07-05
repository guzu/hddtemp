#include <stdlib.h>
#include <iconv.h>
#include <langinfo.h>
#include <locale.h>
#include <string.h>
#include <errno.h>

#include "i18n.h"

static char *iconv_from_utf8_to_locale(const char *string, const char* fallback_string)
{
  const size_t buffer_inc = 80;	// Increment buffer size in 80 bytes step
  const char *charset;
  iconv_t cd;
  size_t nconv;

  char *dest_buffer;
  char *old_dest_buffer;
  char *dest_buffer_ptr;
  char *src_buffer;
  char *src_buffer_ptr;

  size_t dest_buffer_size;
  size_t dest_buffer_size_left;
  size_t src_buffer_size;

  // Get the current charset
  setlocale(LC_ALL, "");
  charset = nl_langinfo (CODESET);

  // Open iconv descriptor
  cd = iconv_open(charset, "UTF-8");
  if (cd == (iconv_t) -1)
     return strdup(fallback_string);

  // Set up the buffer
  dest_buffer_size = dest_buffer_size_left = buffer_inc;
  dest_buffer_ptr = dest_buffer = (char *) malloc(dest_buffer_size);
  src_buffer_ptr = src_buffer = strdup(string);	// work on a copy of the string
  src_buffer_size = strlen(src_buffer) + 1;	// + 1 for \0
  
  // Do the conversion
  while (src_buffer_size != 0)
  {
    nconv = iconv(cd, &src_buffer_ptr, &src_buffer_size, &dest_buffer_ptr, &dest_buffer_size_left);
    if (nconv == (size_t) -1)
    {
      if (errno != E2BIG)   		// exit if translation error
        goto iconv_error;	      
      
      // increase buffer size
      dest_buffer_size += buffer_inc;
      dest_buffer_size_left = buffer_inc;
      old_dest_buffer = dest_buffer;
      dest_buffer = (char *) realloc(dest_buffer, dest_buffer_size);
      if (dest_buffer == NULL)
        goto iconv_error;	      
      dest_buffer_ptr = (dest_buffer_ptr - old_dest_buffer) + dest_buffer;
    }
  }
  iconv_close(cd);			// close descriptor
  free(src_buffer);			// free string
  dest_buffer = (char *) realloc(dest_buffer, dest_buffer_size - dest_buffer_size_left);
  return dest_buffer;

iconv_error:  
  iconv_close(cd);			// close descriptor
  if (dest_buffer != NULL)
    free(dest_buffer);  		// free buffer
  free(src_buffer);			// free string
  return strdup(fallback_string); 	// and return fallback string
}

char *degree_sign()
{
  unsigned char str[] = { 0xc2, 0xb0, 0x00 };

  return iconv_from_utf8_to_locale(str, " \0");
}

