MRuby::Gem::Specification.new('mruby-bcc') do |spec|
  spec.license = 'MIT'
  spec.authors = 'Uchio Kondo'

  spec.add_dependency 'mruby-string-ext', core: 'mruby-string-ext'
  spec.add_dependency 'mruby-io', core: 'mruby-io'
  spec.add_dependency 'mruby-print', core: 'mruby-print'

  spec.linker.libraries << 'bcc'
end
