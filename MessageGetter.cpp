
#include "MessageGetter.h"

CMessageGetter messageGetter;

bool dropFirstLine(string& initial) {
	auto endOfLine = initial.find_first_of('\n');
	if (endOfLine == string::npos) return false;

	initial = initial.substr(endOfLine + 1);

	return true;
}

void getMessageBody(Poco::Net::StreamSocket& sock, string& message) {
	char buffer[4096];
	int result = 0;
	message = "";
	enum { ST_UNKNOWN = 0, ST_OK, ST_ERR } status = ST_UNKNOWN;

	for(;;) {
		result = sock.receiveBytes(buffer, sizeof(buffer) - 1);
		if (result > 0) {
			buffer[result] = '\0';
			message += buffer;

			if (message.size() >= 5) {
				if (status == ST_UNKNOWN) {
					status = ((message.substr(0, 3) == "+OK") ? ST_OK : ST_ERR);
				}

				if ((status == ST_ERR && message[message.size() - 1] == '\n')
				 || (status == ST_OK  && message.substr(message.size() - 5, 5) == "\r\n.\r\n")) {
					
					break;
				}
			}
		} else {
			break;
		}
	}

	// drop last line with term. dot
	if (status == ST_OK) {
		message = message.substr(0, message.size() - 5);
	} else {
		throw wstring(L"При попытке получить сообщение сервер ответил ошибкой: ") + wstring(message.begin(), message.end());
	}
}

void proceedAnswer(Poco::Net::StreamSocket& sock, string& answer) {
	char buffer[1024];
	int result = 0;
	answer = "";

	for(;;) {
		result = sock.receiveBytes(buffer, sizeof(buffer) - 1);
		if (result > 0) {
			buffer[result] = '\0';
			answer += buffer;

			if (answer[answer.size() - 1] == '\n') break;
		} else {
			break;
		}
	}

	if (answer.substr(0, 3) != "+OK") throw wstring(L"Сервер ответил ошибкой: ") + wstring(answer.begin(), answer.end());
}

int sendString(Poco::Net::StreamSocket& sock, std::string string) {
	return sock.sendBytes(string.c_str(), string.size());
}

string wideToNarrow(const wstring& str) {
	return string(str.begin(), str.end());
}

wstring narrowToWide(const string& str) {
	return wstring(str.begin(), str.end());
}

string parseUIDLResponseToFindMessage(string& response, const wstring& messageID) {
	// drop line with +OK response
	dropFirstLine(response);
	response += "\r\n";

	auto position = response.find(string(" ") + wideToNarrow(messageID) + "\r\n");
	if (position == string::npos) {
		throw wstring(L"Такого сообщения в ящике нет");
	}
	unsigned int lineBegin = position;
	while(lineBegin > 0 && isdigit(response[lineBegin - 1])) lineBegin--;

	return response.substr(lineBegin, position - lineBegin);
}

SmartVariant CMessageGetter::getMessage(SmartParameters parameters) {
	wstring server = parameters[0];
	wstring login = parameters[1];
	wstring password = parameters[2];
	wstring messageID = parameters[3];

	auto semicolon = server.find_first_of(L':');
	if (semicolon == wstring::npos) {
		server += L":110";
	}

	string answer;

	try {
		Poco::Net::StreamSocket sock;
		sock.connect(Poco::Net::SocketAddress(wideToNarrow(server)));

		// read hello from server
		proceedAnswer(sock, answer);

		// send USER opcode and wait for an answer
		sendString(sock, string("USER ") + wideToNarrow(login) + "\r\n");
		proceedAnswer(sock, answer);

		// send PASS opcode and wait for an answer
		sendString(sock, string("PASS ") + wideToNarrow(password) + "\r\n");
		proceedAnswer(sock, answer);

		// send UIDL opcode and wait for a list of messages with UIDs
		sendString(sock, "UIDL\r\n");
		getMessageBody(sock, answer);
		
		string messageNumber = parseUIDLResponseToFindMessage(answer, messageID);

		sendString(sock, string("RETR ") + messageNumber + "\r\n");
		getMessageBody(sock, answer);
		dropFirstLine(answer);
	} catch (Poco::Exception& e) {
		throw narrowToWide(e.displayText());
	} catch (exception& e) {
		throw narrowToWide(e.what());
	}

	return BinaryData(answer);
}

