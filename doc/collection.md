MongoDB collection
==================

Methods
-------

### collection:aggregate(pipeline, [options])
Executes an aggregation `pipeline` with `options` on `collection`. Returns a [Cursor].

### collection:count([query], [options])
Executes a count `query` with `options` on `collection`. On success, returns the result. On
failure, returns `nil` and the error message.

### collection:drop([options])
Drops `collection` with `options`. On success, returns `true`. On failure, returns `nil` and the
error message.

### collection:find(query, [options])
Executes a `query` with `options` on `collection`. Returns a [Cursor].

### collection:findAndModify(query, options)
A wrapper for the _findAndModify_ MongoDB command. On success, returns a [BSON document]. On failure,
returns `nil` and the error message.

### collection:insert(document, [flags])
Inserts `document` into `collection`. On success, returns `true`. On failure, returns `nil` and
the error message. Refer to [Insert flags] for `flags` description.

### collection:remove([query], [flags])
Removes documents in `collection` that match `query`. On success, returns `true`. On failure,
returns `nil` and the error message. Refer to [Remove flags] for `flags` description.

### collection:save(document)
Saves `document` into `collection`. If the document has an `_id` field it will be updated.
Otherwise it will be inserted. On success, returns `true`. On failure, returns `nil` and the error
message.

### collection:update(query, update, [flags])
Updates documents in `collection` that match `query`. On success, returns `true`. On failure,
returns `nil` and the error message. Refer to [Update flags] for `flags` description.

### collection:validate([options])
A wrapper for the _validate_ MongoDB command. On success, returns a [BSON document]. On failure,
returns `nil` and the error message.


[BSON document]: bson.md
[Cursor]: cursor.md
[Insert flags]: flags.md#Insert_flags
[Remove flags]: flags.md#Remove_flags
[Update flags]: flags.md#Update_flags
