import threading
import time

# def long_running_task():
#     print("Long-running task started.")
#     # Simulating a time-consuming task
#     time.sleep(10)
#     print("Long-running task completed.")

# # Function to handle user input while the task is running
# def handle_user_input():
#     while True:
#         user_input = input("Enter a command: ")
#         if user_input.lower() == 'stop':
#             # You can add code here to stop the long-running task if needed
#             break
#         else:
#             print(f"Command: {user_input}")

# # Start the long-running task in a separate thread
# task_thread = threading.Thread(target=long_running_task)
# task_thread.start()

# # Handle user input in the main thread
# handle_user_input()

# # Wait for the long-running task to finish
# task_thread.join()

# print("Program exit.")

def pad_word(word, total_length):
    if len(word) >= total_length:
        return word
    else:
        padding_length = total_length - len(word)
        left_padding = padding_length // 2
        right_padding = padding_length - left_padding
        padded_word = '*' * left_padding + word + '*' * right_padding
        return padded_word




def print_decorator(func):
    def wrapper(*args, **kwargs):
        print("==============================")
        result = func(*args, **kwargs)
        print("==============================")
        return result
    return wrapper

@print_decorator
def fancy_communicate(m):
    result = pad_word(m, 30)
    print(result)
    

fancy_communicate('hey there bros')
