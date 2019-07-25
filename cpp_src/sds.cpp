#include "sds.h"





sdshdr::sdshdr(const char *init, size_t initlen): len(initlen), free(0){
    buf = alloc.allocate(initlen+1);
    // auto temp = buf;
    // for (int i = 0; i < initlen; i++)
        // alloc.construct(temp++, init[i]);
    // alloc.construct(temp, '\0');
    memcpy(buf, init, initlen);
    buf[initlen] = '\0';
};

sdshdr::sdshdr(const char *init): sdshdr(init, strlen(init)) {};
sdshdr::sdshdr(): sdshdr("", 0) {};
sdshdr::sdshdr(const sdshdr& other): len(other.len), free(0){
    buf = alloc.allocate(len+1);
    memcpy(buf, other.buf, other.len);
    buf[len] = '\0';

}

sdshdr::sdshdr(long long value): free(0){
    char temp_buf[SDS_LLSTR_SIZE];
    int addlen = sdsll2str(temp_buf, value);
    buf = alloc.allocate(addlen+1);
    memcpy(buf, temp_buf, addlen);
    buf[addlen] = '\0';

    len = addlen;
    
}



sdshdr& sdshdr::operator=(const sdshdr& rhs){
    cout << "operator=" << endl;
    if (this->buf == rhs.get_buf())
        return *this;
    if (len + free >= rhs.len)
        memcpy(this->buf, rhs.get_buf(), rhs.len);
    else{
        auto temp = alloc.allocate(rhs.len+1);
        memcpy(temp, rhs.get_buf(), rhs.len);

        alloc.deallocate(buf, len+free+1);
        buf = temp;
    }
    len = rhs.len;
    free = rhs.free;
    buf[rhs.len] = '\0';
    return *this;
}

sdshdr& sdshdr::operator=(const char* t){
    sdscpylen(t, strlen(t));
    return *this;
}

sdshdr& sdshdr::operator=(long long value){
    char buf[SDS_LLSTR_SIZE];
    int addlen = sdsll2str(buf, value);

    sdscpylen(buf, addlen);
    return *this;

}

sdshdr& sdshdr::operator+=(const sdshdr& rhs){
    this->sdscatlen(rhs.get_buf(), rhs.sdslen());
    return *this;
}
sdshdr& sdshdr::operator+=(const char *t){
    this->sdscatlen(t, strlen(t));
    return *this;
}


sdshdr* operator+(const sdshdr& lhs, const sdshdr& rhs){
    sdshdr* sum = new sdshdr(lhs);
    *sum += rhs;
    return sum;
}

sdshdr* operator+(const sdshdr& lhs, const char* t){
    sdshdr* sum = new sdshdr(lhs);
    *sum += t;
    return sum;
}

ostream& operator<<(ostream& os, const sdshdr &item){
    os << item.get_buf();
    return os;
};



bool operator==(const sdshdr& lhs, const sdshdr& rhs){
    int cmp;
    cmp = lhs.sdscmp(rhs);
    if (cmp == 0) return true;
    return false;
}




// 以下为辅助sds的工具函数
int sdsll2str(char *s, long long value) {
    char *p, aux;
    unsigned long long v;
    size_t l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    v = (value < 0) ? -value : value;
    p = s;
    do {
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);
    if (value < 0) *p++ = '-';

    /* Compute length and add null term. */
    l = p-s;
    *p = '\0';

    /* Reverse the string. */
    p--;
    while(s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}

int sdsull2str(char *s, unsigned long long v) {
    char *p, aux;
    size_t l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    p = s;
    do {
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);

    /* Compute length and add null term. */
    l = p-s;
    *p = '\0';

    /* Reverse the string. */
    p--;
    while(s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}


// sdshdr* sdsjoin(char **argv, int argc, char *sep){
//     auto join = new sdshdr();
    
//     for(int j = 0; j < argc; j++){
//         *join += argv[j];
//         if (j != argc-1) *join += sep;
//     }
//     return join;
// }


sdshdr* sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count){
    int elements = 0, slots = 5, start = 0;

    sdshdr *tokens;

    if (seplen < 1 || len < 0) return NULL;
    // allocator<sdshdr> sds_alloc;
    tokens = new sdshdr[slots];

    if (len == 0){
        *count = 0;
        return tokens;
    }

    for (int j = 0; j < (len - (seplen-1)); j++){
        if (slots < elements+2){
            sdshdr *newtokens;
            int new_slots = slots*2;
            newtokens = new sdshdr[new_slots];
            // newtokens = sds_alloc.allocate(new_slots);
            for (int i = 0; i < slots; i++){
                newtokens[i] = tokens[i];
            }
            delete[] tokens;
            tokens = newtokens;
            slots = new_slots;
        }
        if ((seplen == 1 && *(s+j) == sep[0]) || (memcmp(s+j, sep, seplen) == 0)){
            try{
            tokens[elements] = sdshdr(s+start, j-start);
            
            }
            catch(std::bad_alloc){
                goto cleanup;
            }
            elements++;
            start = j +seplen;
            j = j + seplen -1;
        }
    }
    try{
        tokens[elements] = sdshdr(s+start, len-start);
    }
    catch(std::bad_alloc){
        goto cleanup;
    }
    elements++;
    *count = elements;
    return tokens;
cleanup:
    {
        delete[] tokens;
        *count = 0;
        return NULL;
    }

}

sdshdr* sdsjoin(char **argv, int argc, char *sep){
    auto join = new sdshdr();
    
    for(int j = 0; j < argc; j++){
        *join += argv[j];
        if (j != argc-1) *join += sep;
    }
    return join;
}

int is_hex_digit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

int hex_digit_to_int(char c) {
    switch(c) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': case 'A': return 10;
    case 'b': case 'B': return 11;
    case 'c': case 'C': return 12;
    case 'd': case 'D': return 13;
    case 'e': case 'E': return 14;
    case 'f': case 'F': return 15;
    default: return 0;
    }
}