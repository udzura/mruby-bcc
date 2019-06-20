# mruby-bcc   [![Build Status](https://travis-ci.org/udzura/mruby-bcc.svg?branch=master)](https://travis-ci.org/udzura/mruby-bcc)
BCC class
## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'udzura/mruby-bcc'
end
```
## example
```ruby
p BCC.hi
#=> "hi!!"
t = BCC.new "hello"
p t.hello
#=> "hello"
p t.bye
#=> "hello bye"
```

## License
under the MIT License:
- see LICENSE file
