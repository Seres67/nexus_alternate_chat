# TODO

## Addon-side

- Check channel
- Channel creation => server answer with join_code, update_code(?) and delete_code, these get saved in addon settings file for easy recovery
- Channel update
- Channel deletion
- Join channel
- Leave channel
- Send messages
- Delete messages

## Server-side

- Check channel (GET /channels/:id)
- User creation (no ids, just a username you chose for each channel)
- Channel creation (POST /channels) (generate a unique id, use that to invite other people)
- Channel update (PUT/PATCH /channels/:id)
- Channel deletion (DELETE /channels/:id)
- Join channel (POST /channels/:id/join)
- Leave channel (POST /channels/:id/leave)
- Send messages (websocket?)
- Delete messages (websocket?)


url is prob gonna be https://gw2chat.seres.eu.org