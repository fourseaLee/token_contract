#ifndef CONSUMER_HANDLER_H
#define CONSUMER_HANDLER_H
#include<string>
class ConsumerHandler
{
public:
	ConsumerHandler();
	~ConsumerHandler();

public:
    void UpdateDB(int offset);
    void HandleMsg(std::string msg, int offset);
    void SetTopic(const std::string& topic);

private:
    std::string topic_;




};



#endif
