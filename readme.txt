This program includes the first prototype version of the hex map display. (Currently only shows terrain, not units or coordinates.) To use it, extract the turn report into a file (in the same directory as the program) called REPORT (plain text, no extensions). Then select File/Show map.

This is also the host program which can be used to run games of Rainbow's End.

To start a new game, put the players' email addresses in a file called PLAYERS (plain text, no extension - one email address per line) in the game program directory and select File/New game. (Don't worry if it doesn't seem to have done anything - it's fast enough that there's no perceptible delay.) It'll generate the turn reports in plain text files called 1.R, 2.R, 3.R etc; these should be sent to the players. The game state is saved in a file called GAME.

To run a turn, gather the players' orders into a file called ORDERS (plain text, no extension) in the game program directory and select File/Run turn. (Again, don't worry if nothing seems to have happened - it's too fast to notice a delay.)

While it's possible to send turn reports using an ordinary email program such as Outlook Express, sending a couple of dozen mail messages by hand is a little more tedious than one would like, so there are procedures to automate it. If you don't already have a batch email program, you can check out SndMail which is available for free download from http://xmailserver.org/davide.html. The game program generates a file called SEND.BAT which calls SndMail for each turn report; you'll need to modify it for your own email account, then you can just type SEND once the turn is run.

On the same page there's a program called GetMail which will download a batch of email messages; you can use this for downloading the orders if you so wish.

(The above procedures are clumsier than would ideally be preferred; adding built-in email code to the game program so that everything can be done at the touch of a button is on the to-do list. For the moment, the existing procedures work.)

Questions, comments etc. should be sent to rwallace@esatclear.ie.
