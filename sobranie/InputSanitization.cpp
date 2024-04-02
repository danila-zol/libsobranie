// From mysql/mysys/charset.c

/*
  Escape string with backslashes (\)

  SYNOPSIS
    escape_string_for_mysql()
    charset_info        Charset of the strings
    to                  Buffer for escaped string
    to_length           Length of destination buffer, or 0
    from                The string to escape
    length              The length of the string to escape

  DESCRIPTION
    This escapes the contents of a string by adding backslashes before special
    characters, and turning others into specific escape sequences, such as
    turning newlines into \n and null bytes into \0.

  NOTE
    To maintain compatibility with the old C API, to_length may be 0 to mean
    "big enough"

  RETURN VALUES
    (size_t) -1 The escaped string did not fit in the to buffer
    #           The length of the escaped string
*/

size_t escape_str_for_json(char *to, size_t to_length, const char *from, size_t length)
{
        const char *to_start = to;
        const char *end, *to_end = to_start + (to_length ? to_length - 1 : 2 * length);
        bool overflow = false;

        for (end = from + length; from < end; from++)
        {
                char escape = 0;

                        switch (*from) {
                        case 0:
                                escape = '0';
                                break;
                        case '\\':
                                escape = '\\';
                                break;
                        case '"':
                                escape = '"';
                                break;
                        }
                if (escape)
                {
                        if (to + 2 > to_end)
                        {
                                overflow = true;
                                break;
                        }
                        *to++ = '\\';
                        *to++ = escape;
                }
                else
                {
                        if (to + 1 > to_end)
                        {
                                overflow = true;
                                break;
                        }
                        *to++ = *from;
                }
        }
        *to = 0;
        return overflow ? (size_t) -1 : (size_t) (to - to_start);
}
