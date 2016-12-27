Client class
============

Methods
-------

### client:getCollection(dbname, cname)
Returns a [Collection] object with database name `dbname` and collection name `cname`.

### client:getDatabase(name)
Returns a [Database] object.

### client:getDatabaseNames()
Returns a list of the names of all databases on the server. On error, returns `nil` and the error
message.

### client:getDefaultDatabase()
Returns a [Database] object with the default database name. See [mongo.Client()] for details.


[Collection]: collection.md
[Database]: database.md
[mongo.Client()]: main.md#mongoclienturi
