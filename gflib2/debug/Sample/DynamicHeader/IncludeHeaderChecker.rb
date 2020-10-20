##
## 「特定のディレクトリ以下のファイルがインクルードされていないこと」を確認するスクリプト
##
require "open3"


if (ARGV.length < 2) then
  puts "Usage: Ruby IncludeHeaderChecker.rb [targetProjectfilePath] [forbiddenRootPath(Absolute Path)]"
  exit 1
end

projectFile = ARGV[0]
forbiddenRootDir = ARGV[1].tr("\\", "/")

puts "----------------------"
puts "Check Include Files..."
puts "----------------------"

cmd = <<CMD
@SET Path=C:\\Windows\\Microsoft.NET\\Framework\\v4.0.30319;%Path%
@SET CL=/showIncludes /Zs
@msbuild /nologo /v:m /p:Configuration=Debug;Platform=Win32 #{projectFile} /t:ClCompile
CMD

cmd = cmd.strip.tr("\n", "&")

out, error, status = Open3.capture3(cmd)

isSuccess = true
out.each_line do |str|
  path = str.tr("\\", "/").match(/[:\w\\\/]+.[\w]+/).to_s;

  if path.start_with?(forbiddenRootDir)
    puts "\tWarning! Should NOT include any files under [#{forbiddenRootDir}]"
    puts "\t\t"+path
    isSuccess = false
  end
end

if (isSuccess) then
  puts "\tOK! Any files under [#{forbiddenRootDir}] is not included."
end

exit (isSuccess)


