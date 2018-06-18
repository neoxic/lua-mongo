Client
======

Methods
-------

### client:command(dbname, command, [options], [prefs])
Executes a MongoDB `command` in a database `dbname` and returns the result as a [BSON document]
or [Cursor] handle depending on the command. On error, returns `nil` and the error message.

### client:getCollection(dbname, collname)
Returns a new [Collection] handle with database name `dbname` and collection name `collname`.

### client:getDatabase(dbname)
Returns a new [Database] handle.

### client:getDatabaseNames([options])
Returns a list of the names of all databases on the server. On error, returns `nil` and the error
message.

### client:getDefaultDatabase()
Returns a new [Database] handle with the default database name.

### client:getGridFS(dbname, [prefix])
Creates a MongoDB GridFS instance in database `dbname` and returns its [GridFS] handle.
On error, returns `nil` and the error message.

### client:getReadPrefs()
Returns the default read preferences.

### client:setReadPrefs(prefs)
Sets the default read preferences.


[BSON document]: bson.md
[Collection]: collection.md
[Cursor]: cursor.md
[Database]: database.md
[GridFS]: gridfs.md
