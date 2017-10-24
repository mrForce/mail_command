import socket
ip = socket.gethostbyname('smtp.uconn.edu')
print(ip)

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error as err:
    print('error')
try:
    host = socket.gethostbyname('smtp.uconn.edu')
except socket.gaierror:
    print('bad')
s.connect((host, 25))

def send_command(s, command):
    #returns the response
    print('command: ' + command)
    keepGoing = True
    s.send((command + '\n').encode('utf-8'))
    all_data  = ''
    newline_character_counter = 0
    found_newline = True
    while keepGoing:
        data =  s.recv(1024).decode('utf-8')
        all_data += data
        for c in data:
            if c == '\n':
                found_newline = True
                newline_character_counter = 0
            elif found_newline:
                newline_character_counter += 1
                if newline_character_counter == 4:
                    if c != '-':
                        return all_data
                    else:
                        found_newline = False
                        newline_character_counter = 0


print(send_command(s, 'ehlo "' + socket.getfqdn() + '"'))
print(send_command(s, 'mail from: jordan.force@uconn.edu'))
print(send_command(s, 'rcpt to: 8606349062@vtext.com'))
print(send_command(s, 'data'))
print(send_command(s, 'Hi Jordan, this is just at test\r\n.\r'))
print(send_command(s, 'quit'))

                                                 

s.close()
