#include "email_sender.h"

CURLcode Email::send(const std::string &url,
                     const std::string &userName,
                     const std::string &password)
{
    CURLcode ret = CURLE_OK;

    struct curl_slist *recipients = NULL;

    CURL *curl = curl_easy_init();

    StringData textData { setPayloadText_() };

    if (curl) {
        std::ostringstream cc;
        cc << cc_;

        curl_easy_setopt(curl, CURLOPT_USERNAME,     userName.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD,     password.c_str());
        curl_easy_setopt(curl, CURLOPT_URL,          url     .c_str());

        curl_easy_setopt(curl, CURLOPT_USE_SSL,      (long)CURLUSESSL_ALL);
        //curl_easy_setopt(curl, CURLOPT_CAINFO,       "/path/to/certificate.pem");

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM,    (const char *)from_);
        recipients = curl_slist_append(recipients,   (const char *)to_);
//        recipients = curl_slist_append(recipients,   cc.str().c_str());

        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT,    recipients);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payloadSource_);
        curl_easy_setopt(curl, CURLOPT_READDATA,     &textData);
        curl_easy_setopt(curl, CURLOPT_UPLOAD,       1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE,      1L);

        ret = curl_easy_perform(curl);

        if (ret != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: "
                      << curl_easy_strerror(ret)
                      << std::endl;
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    return ret;
}

std::string Email::dateTimeNow_()
{
    const int RFC5322_TIME_LEN = 32;

    std::string ret;
    ret.resize(RFC5322_TIME_LEN);

    time_t tt;

#ifdef _MSC_VER
    time(&tt);
    tm *t = localtime(&tt);
#else
    tm tv, *t = &tv;
    tt = time(&tt);
    localtime_r(&tt, t);
#endif

    strftime(&ret[0], RFC5322_TIME_LEN, "%a, %d %b %Y %H:%M:%S %z", t);

    return ret;
}

std::string Email::generateMessageId_() const
{
    const size_t MESSAGE_ID_LEN = 37;

    tm t;
    time_t tt;
    time(&tt);

#ifdef _MSC_VER
    gmtime_s(&t, &tt);
#else
    gmtime_r(&tt, &t);
#endif

    std::string ret;
    ret.resize(MESSAGE_ID_LEN);
    size_t dateLen = std::strftime(&ret[0], MESSAGE_ID_LEN, "%Y%m%d%H%M%S", &t);

    static const std::string alphaNum {
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz" };

    std::mt19937 gen;
    std::uniform_int_distribution<> distr(0, alphaNum.length() - 1);
    std::generate_n(ret.begin() + dateLen,
                    MESSAGE_ID_LEN - dateLen,
                    [&]() { return alphaNum[distr(gen)]; });

    return ret;
}

size_t Email::payloadSource_(void *ptr, size_t size, size_t nmemb, void *userp)
{
    StringData *text = reinterpret_cast<StringData *>(userp);

    if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1) || (text->bytesLeft == 0)) {
        return 0;
    }

    if ((nmemb * size) >= text->msg.size()) {
        text->bytesLeft = 0;
        return text->msg.copy(reinterpret_cast<char *>(ptr), text->msg.size());
    }

    return 0;
}

std::string Email::setPayloadText_()
{
    std::string ret = "Date: " + dateTimeNow_() + "\r\n";

    std::ostringstream oss;
    oss << "To: "   << to_   << "\r\n"
           "From: " << from_ << "\r\n"
           "Cc: "   << cc_   << "\r\n";
    ret += oss.str();

    ret +=
        "Message-ID: <" + generateMessageId_() + "@" + from_.domain() + ">\r\n"
        "Subject: " + subject_ + "\r\n"
        "\r\n" +
        body_ + "\r\n"
        "\r\n";

    return ret;
}

std::ostream &operator<<(std::ostream &out, const EmailAddresses &emailAddresses)
{   
    if(emailAddresses.size()!=0){
        out << emailAddresses[0];
        for(int i = 1; i<emailAddresses.size();i++){
            out << emailAddresses[i];
            if(i!=emailAddresses.size()-1){
                out << "," << emailAddresses[i];
            }

        }
    }
    return out;
}

//int main()
//{
//    Email email({ "VIA.demo.ece297@gmail.com", "FromName" },
//                  "lindy.zhai@mail.utoronto.ca",
//                  "Subject",
//                  "Body"  );
//
//    email.send (  "smtp://smtp.gmail.com:25",
//                  "VIA.demo.ece297@gmail.com",
//                  "ece297.demo"  );
//}
