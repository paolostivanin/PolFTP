#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include "../ftputils.h"

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream){
    int written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

char * get_public_ip(void){
    char *ipresult;
    FILE *tmpfp;
    CURL *curl;

    if(((ipresult = malloc(17))) == NULL){
        perror("malloc on pub ip");
        return -1;
    }
    
    curl = curl_easy_init();

    if(!(tmpfp = tmpfile())){
        perror("error on tmp file creation");
        return -1;
    }

    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, "icanhazip.com");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, tmpfp);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    rewind(tmpfp);
    if(fgets(ipresult, 16, tmpfp) == NULL){
        perror("fgets error pubip");
        return -1;
    }
    fclose(tmpfp);
    return ipresult;
}