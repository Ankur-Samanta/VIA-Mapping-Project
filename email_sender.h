#ifndef _EMAIL_SENDER_H_
#define	_EMAIL_SENDER_H_
 
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <random>
#include "curl/curl.h"

class EmailAddress 
{
public:
    EmailAddress(const char *email)
        : email_{std::string(email)}
        {
        }

    EmailAddress(const std::string &email)
        : email_{email.size() ? "<"  + email + ">" : ""}
        {
        }

    EmailAddress(const std::string &email, const std::string &displayName)
        : email_{email.size() ? "<" + email + ">" : ""},
          name_{"\"" + displayName + "\""}
        {
        }

    std::string domain() const
        {
            return email_.substr(email_.find('@') + 1);
        }

    explicit operator const char *() const
        {
            return email_.c_str();
        }

    friend std::ostream &operator<<(std::ostream &out, const EmailAddress &email)
        {
            return out << email.name_ << " " << email.email_;
        }

private:
    std::string email_;
    std::string name_;
};

typedef std::vector<EmailAddress> EmailAddresses;
std::ostream &operator<<(std::ostream &out, const EmailAddresses &emailAddresses);

class Email
{
public:
    Email(const EmailAddress   &from,
          const EmailAddress   &to,
          const std::string    &subject,
          const std::string    &body,
          const EmailAddresses &cc = EmailAddresses())                
        : from_{from}
        , to_{ to }
        , cc_{cc.empty() ? EmailAddresses(1, to) : cc}
        , subject_{subject}
        , body_{body}
        {
        }

    CURLcode send(const std::string &url,
                  const std::string &userName, 
                  const std::string &password);

private:
    struct StringData {
            std::string msg;
            size_t bytesLeft;
            StringData(std::string &&m) : msg{m}, bytesLeft{msg.size()} {}
            StringData(std::string  &m) = delete;
        };

    static std::string dateTimeNow_();
    static size_t payloadSource_(void *ptr, size_t size, size_t nmemb, void *userp);
    std::string generateMessageId_() const;
    std::string setPayloadText_();

    EmailAddress from_, to_;
    EmailAddresses cc_;
    std::string subject_, body_;
};


#endif 