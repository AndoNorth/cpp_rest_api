import requests

BASE= "http://127.0.0.1:5000/"

data = [{"likes": 10, "name":"ando's funny video", "views" : 121},
        {"likes": 1420, "name":"how to make paper planes", "views" : 42000},
        {"likes": 23099, "name":"awesome flip", "views" : 929299}]

for i in range(len(data)):
    # response = requests.put(BASE + "video/"+str(i), data[i])
    x = i
request_str=BASE + "videos/1"
print(request_str)
response = requests.get(request_str)
print(response)
response = requests.put(request_str)
print(response)
response = requests.post(request_str)
print(response)
response = requests.delete(request_str)
print(response)
