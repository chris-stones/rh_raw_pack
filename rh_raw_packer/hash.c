/****
 * not used for encryption.
 * If there is a collosion, we will know about it at build time.
 * All I care about here is speed and bug-free-ness, and this is too small to be buggy or overly slow!
 *
 * This code was written by this guy -> http://research.microsoft.com/en-us/people/palarson/
 * I found it on Stack-overflow.
 *
 * THANKS PAUL LARSON.
 **/
unsigned int hash( const char* s, unsigned int seed)
{
    unsigned hash = seed;
    while (*s)
    {
        hash = hash * 101  +  *s++;
    }
    return hash;
}

