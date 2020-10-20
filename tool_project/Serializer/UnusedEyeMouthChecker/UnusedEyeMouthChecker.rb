require "open3"

seriesDirectoryList = Dir.glob("*/") & ["rg/", "gs/", "rs/", "dp/", "bw/", "kujira/", "sango/", "niji/"]
for seriesDirectory in seriesDirectoryList do
	pokemonDirectoryList = Dir.glob(seriesDirectory + "*/")
	for pokemonDirectory in pokemonDirectoryList
		pid = system("UnusedEyeMouthChecker.exe #{Dir.pwd}/#{pokemonDirectory.chop}")
	end
end
