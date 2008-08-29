@echo "Welcome to OblivionOnline"
@set /p IP=Please Enter an IP Address of the computer running the server, Can be the same as this(127.0.0.1 for your own PC):
@set /p Port=Please Enter a Port (41805 if you do not know it): 
@del realmlist.wth
@echo 1 %IP% %Port% >> realmlist.wth
@pause "Your settings have been saved. You can now press enter to launch the game and then start the obse_loader at any time to reconnect to the same IP
@obse_loader