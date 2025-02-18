#include<bits/stdc++.h>
#include<curl/curl.h>
#include<nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

string extract_text_from_api_response(const string& response) {
    json response_from_api = json::parse(response);
    return response_from_api["candidates"][0]["content"]["parts"][0]["text"];
}

int main() {
    string response_string;
    CURL *curl = curl_easy_init();
    if(!curl) {
        cout << stderr << "The curl function initialization failed!";
        return -1;
    } else {
        cout << "The cURL initialization was successful!!";
        cout << "\nPlease enter the topic upon which you want us to generate the flash cards: ";
        string topic;
        getline(cin, topic);

        string json_format_string = R"({
  "contents": [
    {
      "parts": [
        {
          "text": "Generate 5 flashcards on )" + topic + R"(. "
        }
      ]
    }
  ]
})";

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        const char* api_key = getenv("GEMINI_API_KEY");
        if (!api_key) {
            cerr << "ERROR: API key not found! Make sure GEMINI_API_KEY is set." << endl;
            return -1;
        } else {
            cout << "API key found and is not shown due to security reasons" << endl;
        }

        string urler = "https://generativelanguage.googleapis.com/v1/models/gemini-pro:generateContent?key=" + string(api_key);
        
        curl_easy_setopt(curl, CURLOPT_URL, urler.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_format_string.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            cout << "\nThe Gemini API URL setup failed to establish a connection " << curl_easy_strerror(res);
            return -1;
        } else {
            string final_response_format = extract_text_from_api_response(response_string);
            cout << "\nThe flashcards so generated are:- " << final_response_format << endl;
        }
        curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);
    return 0;
}
