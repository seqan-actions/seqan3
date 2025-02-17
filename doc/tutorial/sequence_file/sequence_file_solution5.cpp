#include <seqan3/test/snippet/create_temporary_snippet_file.hpp>
seqan3::test::create_temporary_snippet_file my_fastq
{
    "my.fastq",
R"//![fastq_file](
@seq1
CGATCGATC
+
IIIIIIIII
@seq2
AGCG
+
IIII
@seq3
AGCTAGCAGCGATCG
+
IIIIIHIIJJIIIII
@seq4
AGC
+
III
@seq5
AGCTAGCAGCGATCG
+
IIIIIHIIJJIIIII
)//![fastq_file]"
}; // std::filesystem::current_path() / "my.fastq" will be deleted after the execution

// std::filesystem::current_path() / "output.fastq" will be deleted after the execution
seqan3::test::create_temporary_snippet_file output{"output.fastq", ""};

//![solution]
#include <seqan3/std/algorithm>
#include <filesystem>
#include <seqan3/std/ranges>

#include <seqan3/io/sequence_file/all.hpp>

int main()
{
    std::filesystem::path current_path = std::filesystem::current_path();

    seqan3::sequence_file_input fin{current_path / "my.fastq"};
    seqan3::sequence_file_output fout{current_path / "output.fastq"};

    auto length_filter = std::views::filter([] (auto & rec)
    {
        return std::ranges::size(rec.sequence()) >= 5;
    });

    fout = fin | length_filter;

    // This would also work: copy an input range into an output range
    std::ranges::move(fin | length_filter, fout.begin());
}
//![solution]
