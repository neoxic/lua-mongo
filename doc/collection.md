Collection
==========

Methods
-------

### collection:aggregate(pipeline, [options])
Executes an aggregation `pipeline` on `collection` and returns a [Cursor] object.

### collection:createBulkOperation([ordered = true])
Returns a new [Bulk operation].

_Ordered_ bulk operations are batched and sent to the server in the order suitable for serial
execution. The processing aborts when the first error is encountered.

_Unordered_ bulk operations are batched and sent to the server in arbitrary order where they may
be executed in parallel with any errors reported after all operations are attempted.

### collection:count([query], [options])
Executes a count `query` on `collection` and returns the result. On error, returns `nil` and the
error message.

### collection:drop([options])
Drops `collection` and returns `true`. On error, returns `nil` and the error message.

### collection:find(query, [options])
Executes a find `query` with `options` on `collection` and returns a [Cursor] object.

### collection:findAndModify(query, options)
Executes a find-and-modify `query` on `collection` and returns the result as a [BSON document] or
[BSON Null][BSON type] if nothing was found. On error, returns `nil` and the error message.

### collection:getName()
Returns the name of `collection`.

### collection:insert(document, [flags])
Inserts `document` into `collection` and returns `true`. On error, returns `nil` and the error
message. See also [Flags for insert] for information on `flags`.

If the document does not have an `_id` field, a new unique [BSON ObjectID][BSON type] will be
generated locally and added to the document. If you must know the inserted document's `_id`,
generate it in your code and add to the document before calling this method.

### collection:remove([query], [flags])
Removes documents in `collection` that match `query` and returns `true`. On error, returns `nil`
and the error message. See also [Flags for remove] for information on `flags`.

### collection:save(document)
Saves `document` into `collection` and returns `true`. On error, returns `nil` and the error
message. If the document has an `_id` field, it will be updated. Otherwise, it will be inserted.

### collection:update(query, document, [flags])
Updates documents in `collection` that match `query` with `document` and returns `true`. On error,
returns `nil` and the error message. See also [Flags for update] for information on `flags`.

### collection:validate([options])
Validates `collection` and returns the result as a [BSON document]. On error, returns `nil` and
the error message.


[BSON document]: bson.md
[BSON type]: bsontype.md
[Bulk operation]: bulkoperation.md
[Cursor]: cursor.md
[Flags for insert]: flags.md#flags-for-insert
[Flags for remove]: flags.md#flags-for-remove
[Flags for update]: flags.md#flags-for-update
