Database
========

Methods
-------

### database:addUser(username, password, [roles], [extra])
Creates a new user with access to `database`. On error, returns `nil` and the error message.

### database:createCollection(collname, [options])
Explicitly creates a new collection `collname` in `database` and returns its [Collection] handle.
On error, returns `nil` and the error message.

### database:drop([options])
Drops `database` and returns `true`. On error, returns `nil` and the error message.

### database:getCollection(collname)
Returns a new [Collection] handle.

### database:getCollectionNames([options])
Returns a list of the names of all collections in `database`. On error, returns `nil` and the error
message.

### database:getName()
Returns the name of `database`.

### database:getReadPrefs()
Returns the default read preferences.

### database:hasCollection(collname)
Checks if a collection `collname` exists on the server within `database`. Returns `true` if the
collection exists or `nil` if it does not exist. On error, returns `nil` and the error message.

### database:removeAllUsers()
Removes all users from `database` and returns `true`. On error, returns `nil` and the error message.

### database:removeUser(username)
Removes a user from `database` and returns `true`. On error, returns `nil` and the error message.

### database:setReadPrefs(prefs)
Sets the default read preferences.


[Collection]: collection.md
