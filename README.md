# IRC
An Internet Relay Chat (IRC) server.

## Usage:

1.
Git clone the repository and install a C++ compiler.
For Ubuntu for example:
```
sudo apt install g++
```

2.
In the root directory type "*make*".<br>
To start the IRC server, type "*./ircserv port password*" where *port* is the listening port number and *password* is the password the clients need to join the IRC server.<br>
The server will then start and wait for incoming connections.<br>

3.
To stop the IRC server, press "*ctrl+c*" or "*ctrl+\\*".<br>
To delete all binary files, type "*make fclean*".<br>


## Connection to the server using IRC client irssi:

1.
Install the IRC client irssi.<br>
```
sudo apt install irssi
```

2.
To connect to the server run irssi:
```
irsi -n your_nickname
```
And then type the command:
```
/connect localhost port password
```

3.
Display list of the channels:
```
/list -yes
```

4.
Create a new channel or join the channel if it already exists:
```
/join channelname
```

5.
Change channel mode to invite only:
```
/mode +i
```

6.
Change the channels topic:
```
/topic Topic description here.
```

7.
Invite a user to the current channel:
```
/invite nickname
```

8.
List nicknames of the users in the channel:
```
/names
```

9.
Kick a user from a channel:
```
/kick nickname
```

10.
Send a private message:
```
/msg nickname Your message here.
```

11.
Leave irssi:
```
/quit
```

Docs:
https://irssi.org/documentation/help/cat_Core%20Commands/<br>
Change window in irssi:
```
Ctrl + n
```


## Connection to the server using nc command:

1.
To connect to the IRC server with a client, open a new terminal and type "*nc localhost port*"<br>

2.
```
PASS your_password
NICK your_nickname
USER username hostname servername :realname
```

3.
Display list of the channels:
```
LIST
```

4.
Create a new channel or join the channel if it already exists:
```
JOIN #channelname
```

5.
Change channel mode to invite only:
```
MODE #channelname +i
```

6.
Change the channels topic:
```
TOPIC #channelname :Topicdescriptionhere.
```

7.
Invite a user to the current channel:
```
INVITE nickname #channelname
```

8.
List nicknames of the users:
```
NAMES
```

9.
Kick a user from a channel:
```
KICK #channelname othernick
```

10.
Send a private message:
PRIVMSG recipient_nickname :Your message here

11.
Leave the IRC server:
```
Ctrl + c
```

Docs:
https://datatracker.ietf.org/doc/html/rfc1459<br>
