#include <like_match.h>

int MatchText(char* t, int tlen, char* p, int plen)
{
    /* Fast path for match-everything pattern */
    if (plen == 1 && *p == '%')
        return LIKE_TRUE;

    /* Since this function recurses, it could be driven to stack overflow */
    /*check_stack_depth();*/

    /*
     * In this loop, we advance by char when matching wildcards (and thus on
     * recursive entry to this function we are properly char-synced). On other
     * occasions it is safe to advance by byte, as the text and pattern will
     * be in lockstep. This allows us to perform all comparisons between the
     * text and pattern on a byte by byte basis, even for multi-byte
     * encodings.
     */
    while (tlen > 0 && plen > 0) {
        if (*p == '\\') {
            /* Next pattern byte must match literally, whatever it is */
            NextByte(p, plen);
            /* ... and there had better be one, per SQL standard */
            if (plen <= 0)
                return LIKE_ABORT;
            if (GETCHAR(*p) != GETCHAR(*t))
                return LIKE_FALSE;
        } else if (*p == '%') {
            char firstpat;

            /*
             * % processing is essentially a search for a text position at
             * which the remainder of the text matches the remainder of the
             * pattern, using a recursive call to check each potential match.
             *
             * If there are wildcards immediately following the %, we can skip
             * over them first, using the idea that any sequence of N _'s and
             * one or more %'s is equivalent to N _'s and one % (ie, it will
             * match any sequence of at least N text characters).  In this way
             * we will always run the recursive search loop using a pattern
             * fragment that begins with a literal character-to-match, thereby
             * not recursing more than we have to.
             */
            NextByte(p, plen);

            while (plen > 0) {
                if (*p == '%')
                    NextByte(p, plen);
                else if (*p == '_') {
                    /* If not enough text left to match the pattern, ABORT */
                    if (tlen <= 0)
                        return LIKE_ABORT;
                    NextChar(t, tlen);
                    NextByte(p, plen);
                } else
                    break; /* Reached a non-wildcard pattern char */
            }

            /*
             * If we're at end of pattern, match: we have a trailing % which
             * matches any remaining text string.
             */
            if (plen <= 0)
                return LIKE_TRUE;

            /*
             * Otherwise, scan for a text position at which we can match the
             * rest of the pattern.  The first remaining pattern char is known
             * to be a regular or escaped literal character, so we can compare
             * the first pattern byte to each text byte to avoid recursing
             * more than we have to.  This fact also guarantees that we don't
             * have to consider a match to the zero-length substring at the
             * end of the text.
             */
            if (*p == '\\') {
                if (plen < 2)
                    return LIKE_ABORT;
                firstpat = GETCHAR(p[1]);
            } else
                firstpat = GETCHAR(*p);

            while (tlen > 0) {
                if (GETCHAR(*t) == firstpat) {
                    int matched = MatchText(t, tlen, p, plen);

                    if (matched != LIKE_FALSE)
                        return matched; /* TRUE or ABORT */
                }

                NextChar(t, tlen);
            }

            /*
             * End of text with no match, so no point in trying later places
             * to start matching this pattern.
             */
            return LIKE_ABORT;
        } else if (*p == '_') {
            /* _ matches any single character, and we know there is one */
            NextChar(t, tlen);
            NextByte(p, plen);
            continue;
        } else if (GETCHAR(*p) != GETCHAR(*t)) {
            /* non-wildcard pattern char fails to match text char */
            return LIKE_FALSE;
        }

        /*
         * Pattern and text match, so advance.
         *
         * It is safe to use NextByte instead of NextChar here, even for
         * multi-byte character sets, because we are not following immediately
         * after a wildcard character. If we are in the middle of a multibyte
         * character, we must already have matched at least one byte of the
         * character from both text and pattern; so we cannot get out-of-sync
         * on character boundaries.  And we know that no backend-legal
         * encoding allows ASCII characters such as '%' to appear as non-first
         * bytes of characters, so we won't mistakenly detect a new wildcard.
         */
        NextByte(t, tlen);
        NextByte(p, plen);
    }

    if (tlen > 0)
        return LIKE_FALSE; /* end of pattern, but not of text */

    /*
     * End of text, but perhaps not of pattern.  Match iff the remaining
     * pattern can match a zero-length string, ie, it's zero or more %'s.
     */
    while (plen > 0 && *p == '%')
        NextByte(p, plen);
    if (plen <= 0)
        return LIKE_TRUE;

    /*
     * End of text with no match, so no point in trying later places to start
     * matching this pattern.
     */
    return LIKE_ABORT;
}
