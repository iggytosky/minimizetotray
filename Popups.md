# Introduction #

Other extension can show popup notifications usig **Minimize to tray** extension.

<img src='http://sites.google.com/site/minimizechrometotray/_/rsrc/1257852471935/home/popup.png' />

# Details #

You can add popup method to show popup notification:

function popup(text)
{
> // Connect to extension

> var port = chrome.extension.connect("ppkfenalijoglfhgpchdegciehdlinkh", {name: 'minimize-to-tray'});

> // Send command to show up popup notification

> port.postMessage({command: 'PopupNotify', title: "", text: text});
}

and call that method when you need inform user with popup:

if(unreadCount != 0)
{
> popup("You have " + unreadCount + " unreaden message(s)!");
}