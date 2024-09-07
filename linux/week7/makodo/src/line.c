#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>

extern char token[100];

void lineNotify(char *message) {
    CURL *curl;
    CURLcode res;
    char error[CURL_ERROR_SIZE];
    char url[50]="https://notify-api.line.me/api/notify";
    char header[100]={0};
    struct curl_slist *list=NULL;
    struct curl_httppost *CurlFormpost = NULL;
    struct curl_httppost *CurlLastptr = NULL;
    long CurlStatus;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        sprintf(header, "Authorization: Bearer %s", token);
        list = curl_slist_append(list, header);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        curl_formadd(&CurlFormpost, &CurlLastptr,
		    CURLFORM_COPYNAME, "message",
		    CURLFORM_COPYCONTENTS, message,
		    CURLFORM_END);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, CurlFormpost);    
        res = curl_easy_perform(curl);	
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &CurlStatus);;
			if (CurlStatus == 200)
			{
				//Success
				//Response from server is in: CurlResponseString
            }
        }
        curl_easy_cleanup(curl);
        curl_formfree(CurlFormpost);
    }
    return 0;
}

