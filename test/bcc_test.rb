##
## BCC Test
##

assert("BCC#hello") do
  t = BCC.new "hello"
  assert_equal("hello", t.hello)
end

assert("BCC#bye") do
  t = BCC.new "hello"
  assert_equal("hello bye", t.bye)
end

assert("BCC.hi") do
  assert_equal("hi!!", BCC.hi)
end
