Client
======

Methods
-------

### client:command(dbname, command)
Executes a MongoDB `command` in a database `dbname` and returns the result as a [BSON document]
or [Cursor] object depending on the command. On error, returns `nil` and the error message.

### client:getCollection(dbname, collname)
Returns a new [Collection] object with database name `dbname` and collection name `collname`.

### client:getDatabase(dbname)
Returns a new [Database] object.

### client:getDatabaseNames()
Returns a list of the names of all databases on the server. On error, returns `nil` and the error
message.

### client:getDefaultDatabase()
Returns a new [Database] object with the default database name.

### client:getGridFS(dbname, [prefix])
Creates a MongoDB GridFS instance in database `dbname` and returns a corresponding [GridFS] object.
On error, returns `nil` and the error message.


[Collection]: collection.md
[Database]: database.md
[GridFS]: gridfs.md
