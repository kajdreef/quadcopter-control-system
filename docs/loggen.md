# Logging

Implement a log message (or replace DEB_mes) to be able to communicate to the QR when logging needs to start/stop and when it needs to start sending data.

## Requirements
1. Sending log is dependent on state (ONLY SAFE MODE)
2. During sending logging you're not allowed to switch modes
3. Start/Stop can always be sent and is independent of state

## Sending
1. Send message that starts logging.
2. Send message that stops logging.
3. Send message that starts the sending of log message.

## Receiving
1. After sending message to QR the system will start a while loop to obtain data

## Advantage
1. Relatively easy to implement in current program

## Disadvantage
1. While loop will make the system unresponsive until all the log data has been received.
