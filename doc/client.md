Client
======

Methods
-------

### client:command(dbname, command)
Executes a MongoDB `command` in a database `dbname` and returns the result as a [BSON document]
or a [Cursor] object depending on the command. On error, returns `nil` and the error message.

### client:getCollection(dbname, cname)
Returns a [Collection] object with database name `dbname` and collection name `cname`.

### client:getDatabase(name)
Returns a [Database] object.

### client:getDatabaseNames()
Returns a list of the names of all databases on the server. On error, returns `nil` and the error
message.

### client:getDefaultDatabase()
Returns a [Database] object with the default database name.


[Collection]: collection.md
[Database]: database.md
