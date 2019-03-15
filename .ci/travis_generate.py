#!/usr/bin/env python3

from travis_helper import job, properties, render_template

def addon_coverity():
	return {
		"coverity_scan": {
			"name": "smtrat/carl",
			"description": "CArL",
			"properties": {
				"notification_email": "gereon.kremer@cs.rwth-aachen.de",
				"build_command_prepend": "cov-configure --template --comptype clangcc --compiler $CXX",
				"build_command": ".ci/build.sh",
				"branch_pattern": "master",
			}
		}
	}

def addon_sonarcloud():
	return {
		"sonarcloud": {
			"organization": "smtrat-github",
			"token": "nIGn6M7vkwD6HAKgS94QZIIU+A+dWOgqXzJ7lnAdGLXUx3cStVMO1LuOANttGyeGSJNj8Fa+YzwCx5EMQDvZW/b8cuoRld+I4gbmszUB6BXwQ6JJvpFczHrPpwyeo2LKrBN549aBCtOaLzw7rVPDzcdC6T39IvxpPXVCMTTjoq7Mp12HSWS8Ra8YIsOnJfYKVSxjCwcY9ICac70zpA6uKuWBNL13EBM+IpLACLFDKMcaIdb2CGyRvtbt7u8BOU9mjulRtpg1Ndc3eGEIIJJXM8lQTA+iMB6iapGWYbMB5Gwifrwy59UTgNbdR/6sWP5E5kxBGxn1lyp9VP6ChSS/b3Szhh0jUWaqBxoAK0Kh4KBeW7eeLvaUALuPmoNneGUZACrbNDq6aVzHUgwEKQTxF0reDkG3ZaEU+1NCukvLaI58OBxenb5bMOlEWzUMSMMuNO0MgVKXc3Nvr4oEm0USP6Ixky1AUTKTVDY87HHuQ+kCM/L5MQUQTwtQPuWF1zkDry+6A2LNABySla9AAtxlUth7rGvLwaTz2o3yMOIohQb12r8LqXnjESVcENk0f0gbyqeqM7aPcXAyqc6YDW9LBDSsWWa9SqxEfwz2zktzsWfKfCZWi4Fn7CaPdHGsGlSaGsXGovrT1DbyQPiTND0R1cinfrOqZBgwjWOB6JTol+g="
		}
	}

properties.update({
	"addon.coverity": {"addons": addon_coverity()},
	"addon.sonarcloud": {"addons": addon_sonarcloud()},
})

jobs = [
	job("0-clang", ["build", "linux", "clang-5.0", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-6.0", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-7.0", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-8.0", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-9.0", "build.sh", "mayfail"]),
	job("1-gcc", ["dependencies", "linux", "g++-7", "build.sh"]),
	job("1-gcc", ["build", "linux", "g++-7", "j1", "build.sh"]),
	job("1-gcc", ["dependencies", "linux", "g++-8", "build.sh"]),
	job("1-gcc", ["build", "linux", "g++-8", "j1", "build.sh"]),
	job("2-macos", ["build", "xcode9.3", "build.sh"]),
	job("2-macos", ["build", "xcode9.4", "build.sh"]),
	job("2-macos", ["build", "xcode10", "build.sh"]),
	job("2-macos", ["build", "xcode10.1", "build.sh"]),
	job("3-docs", ["build", "linux", "g++-8", "task.doxygen", "j1", "build.sh"]),
	job("4-tidy", ["check", "linux", "clang-9.0", "task.tidy", "build.sh", "mayfail"]),
	job("5-checker", ["dependencies", "linux", "clang-7.0", "task.coverity", "build.sh"]),
	job("5-checker", ["check", "linux", "clang-7.0", "task.coverity", "addon.coverity", "mayfail"]),
	job("5-checker", ["dependencies", "linux", "clang-7.0", "task.sonarcloud", "addon.sonarcloud", "build.sh", "mayfail"]),
	job("5-checker", ["build", "linux", "clang-7.0", "task.sonarcloud", "task.sonarcloud-build", "addon.sonarcloud", "build.sh", "mayfail"]),
	job("5-checker", ["check", "linux", "clang-7.0", "task.sonarcloud", "task.sonarcloud-scan", "addon.sonarcloud", "build.sh", "mayfail"]),
	job("5-checker", ["check", "linux", "clang-9.0", "task.clang-ubsan", "build.sh", "mayfail"]),
	job("6-addons", ["dependencies", "linux", "g++-7", "task.pycarl", "j1", "build.sh"]),
	job("6-addons", ["build", "linux", "g++-7", "task.pycarl", "j1", "build.sh", "mayfail"]),
	job("6-addons", ["dependencies", "linux", "g++-7", "task.addons", "j1", "build.sh"]),
	job("6-addons", ["build", "linux", "g++-7", "task.addons", "j1", "build.sh", "mayfail"]),
]

cached = [
	"$HOME/usr/",
	"$HOME/.sonar/cache",
	"build/resources",
]

render_template(jobs, cached)
