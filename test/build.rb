#!/usr/bin/env ruby

require'optparse'

opts = {
  entt: nil,
  cc: 'clang++',
  output: 'box2d-mruby',
  box2d: nil,
  box2d_build: 'build/src',
  l: ['box2d']
}

OptionParser.new do |o|
  o.on '--entt=VALUE', 'path to entt directory' do |entt|
    opts[:entt] = entt
  end

  o.on '--cc=COMPILER', 'c++ compiler' do |cc|
    opts[:cc] = cc
  end

  o.on '--output=BINARYNAME', 'name of binary output' do |file|
    opts[:output] = file
  end

  o.on '--cfiles=FILES', 'c++ source files (space separated)' do |cfiles|
    opts[:cfiles] = cfiles
  end

  o.on '-I=DIR', 'c++ header file dir' do |dir|
    (opts[:I] ||= []) << dir
  end

  o.on '-l library', 'library to link with' do |lib|
    opts[:l] << lib
  end

  o.on '--box2d=DIR', 'path to box2d' do |dir|
    opts[:box2d] = dir
  end

  o.on '--box2d-build=type', "type of build (default: #{opts[:box2d_build]})" do |build|
    opts[:box2d_build] = build
  end

end.parse!


opts[:cfiles] ||= "#{opts[:output]}.cc"

fail = false
opts.each {|k,v|
  if v.nil?
    fail = true
    puts "Missing option: #{k}"
  end
}
abort if fail

cmd = "#{opts[:cc]} \
  -g -std=c++1z \
  -I #{opts[:entt]} \
  -I #{opts[:box2d]}/include \
  #{`pkg-config --cflags sfml-graphics`.strip} \
  #{opts[:I].map{|dir| "-I#{dir}"}.join(' ') if opts[:I]} \
  -I ../include \
  #{opts[:cfiles]} \
  -o #{opts[:output]} \
  -L #{opts[:box2d]}/#{opts[:box2d_build]} \
  #{opts[:l].map{|lib| "-l#{lib}"}.join(' ')} \
  #{`pkg-config --libs sfml-graphics`.strip}"

puts "Running command: #{cmd}"
exit Kernel.system cmd
