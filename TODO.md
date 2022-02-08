
# Runs
- Add runs to end
- Delete selected run
- Move run up/down
- insert run in the middle
- Change run class+car

### timing / state machine
- Assign start and sector times to runs in order


# Timing
- Global "current" event
  - update protobuf
  - update database
  - remove Event combo box from Registration window
  - Registraton widget should subscribe to "event" events so when current changes, we can switch
  - Show current event in the top bar next to icon
  - maybe some sort of like singleton function/class we can read-from/subscribe-to for current event?
  
# Random stuff
- Move the "import" inserts into the "save" button on the event, rather than the import.
- changing event in registration window


############
# FUTURE
- awesome bar
 - http://nazmus-sadat.com/blog/2015/11/15/qt-customizing-qcompleter/
 - https://doc.qt.io/qt-5/qtwidgets-itemviews-customsortfiltermodel-example.html
 - https://doc.qt.io/qt-5/qtnetwork-googlesuggest-example.html

