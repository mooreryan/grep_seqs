#!/usr/bin/env ruby
Signal.trap("PIPE", "EXIT")
require "parse_fasta"
require "set"
require "abort_if"

include AbortIf

unless ARGV.count == 2
  abort "\nusage: ruby subtract_seqs.rb f1.fasta f2.fasta > seqs_unique_to_f1.fasta\n\nIt works on the ID part only!!! Ignores anything after the first space in the header!!!\n\n"
end

main_file = ARGV[0]
subtract_file = ARGV[1]

subtract_ids = Set.new
ParseFasta::SeqFile.open(subtract_file).each_record do |rec|
  subtract_ids << rec.id
end

ParseFasta::SeqFile.open(main_file).each_record do |rec|
  unless subtract_ids.include? rec.id
    puts rec
  end
end
