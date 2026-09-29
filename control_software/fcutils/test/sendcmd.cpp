#include <iostream>
#include <mqueue.h>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./message_sender <message>\n";
        return 1;
    }

    const char* queue_name = "/CStoRC";  // Specify the name of the message queue

    // Open the message queue for writing
    mqd_t mq = mq_open(queue_name, O_WRONLY);
    if (mq == -1) {
        perror("mq_open");
        return 1;
    }

    // Get the message from the command line argument
    const char* message = argv[1];
    size_t message_len = strlen(message);

    // Send the message to the message queue
    if (mq_send(mq, message, message_len, 0) == -1) {
        perror("mq_send");
        return 1;
    }

    // Close the message queue
    if (mq_close(mq) == -1) {
        perror("mq_close");
        return 1;
    }

    std::cout << "Message sent to the queue successfully.\n";

    return 0;
}
