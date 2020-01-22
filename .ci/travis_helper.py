#!/usr/bin/env python3

from jinja2 import Environment, FileSystemLoader
import os

def datamerge(a, b):
	"""Merges two lists or dictionaries."""
	if isinstance(a, list) and isinstance(b, list):
		return a + b
	elif isinstance(a, dict) and isinstance(b, dict):
		res = a.copy()
		for k,v in b.items():
			if k in res:
				res[k] = datamerge(res[k], v)
			else:
				res[k] = v
		return res
	else:
		return b



def addon_apt(sources, packages):
	"""Standard addon for apt."""
	return {
		"apt": {
			"sources": sources,
			"packages": packages
		}
	}

def addon_brew(packages):
	"""Standard addon for homebrew."""
	return {
		"homebrew": {
			"packages": packages
		}
	}

def addon_cache(*dirs):
	"""Standard addon for caching."""
	return {
		"cache": dirs
	}

# List of predefined properties.
# Can be extended.
properties = {
	"dependencies": {"stage": "dependencies", "script": ["TASK=dependencies"]},
	"build": {"stage": "build"},
	"check": {"stage": "check"},

	"xcode9.1": {"os": "osx", "osx_image": "xcode9.1", "env": [], "addons": addon_brew(["llvm"])},
	"xcode9.2": {"os": "osx", "osx_image": "xcode9.2", "env": [], "addons": addon_brew(["llvm"])},	
	"xcode9.3": {"os": "osx", "osx_image": "xcode9.3", "env": [], "addons": addon_brew(["llvm"])},
	"xcode9.4": {"os": "osx", "osx_image": "xcode9.4", "env": [], "addons": addon_brew(["llvm"])},
	"xcode10": {"os": "osx", "osx_image": "xcode10", "env": [], "addons": addon_brew(["llvm"])},
	"xcode10.1": {"os": "osx", "osx_image": "xcode10.1", "env": [], "addons": addon_brew(["llvm"])},
	"xcode10.2": {"os": "osx", "osx_image": "xcode10.2", "env": [], "addons": addon_brew(["llvm"])},
	"xcode10.3": {"os": "osx", "osx_image": "xcode10.3", "env": [], "addons": addon_brew(["llvm"])},
	"xcode11": {"os": "osx", "osx_image": "xcode11", "env": []},
	"xcode11.1": {"os": "osx", "osx_image": "xcode11.1", "env": []},
	"xcode11.2": {"os": "osx", "osx_image": "xcode11.2", "env": []},
	"xcode11.3": {"os": "osx", "osx_image": "xcode11.3", "env": []},
	"linux": {"os": "linux"},

	"clang-3.8": {"env": ["CC=clang-3.8 CXX=clang++-3.8"], "compiler": "clang++-3.8", "addons": addon_apt(["llvm-toolchain-xenial-3.8"], ["clang-3.8", "libstdc++-6-dev"])},
	"clang-3.9": {"env": ["CC=clang-3.9 CXX=clang++-3.9"], "compiler": "clang++-3.9", "addons": addon_apt(["llvm-toolchain-xenial-3.9"], ["clang-3.9", "libstdc++-6-dev"])},
	"clang-4.0": {"env": ["CC=clang-4.0 CXX=clang++-4.0"], "compiler": "clang++-4.0", "addons": addon_apt(["llvm-toolchain-xenial-4.0"], ["clang-4.0", "libstdc++-6-dev"])},
	"clang-5.0": {"env": ["CC=clang-5.0 CXX=clang++-5.0"], "compiler": "clang++-5.0", "addons": addon_apt([], ["clang-5.0"])},
	"clang-6.0": {"env": ["CC=clang-6.0 CXX=clang++-6.0"], "compiler": "clang++-6.0", "addons": addon_apt([], ["clang-6.0"])},
	"clang-7.0": {"env": ["CC=clang-7 CXX=clang++-7"], "compiler": "clang++-7.0", "addons": addon_apt([{"sourceline": "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic main", "key_url": "https://apt.llvm.org/llvm-snapshot.gpg.key"}], ["clang-7", "libstdc++-8-dev"])},
	"clang-8.0": {"env": ["CC=clang-8 CXX=clang++-8"], "compiler": "clang++-8.0", "addons": addon_apt([{"sourceline": "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-8 main", "key_url": "https://apt.llvm.org/llvm-snapshot.gpg.key"}], ["clang-8", "libstdc++-8-dev"])},
	"clang-9.0": {"env": ["CC=clang-9 CXX=clang++-9"], "compiler": "clang++-9.0", "addons": addon_apt([{"sourceline": "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-9 main", "key_url": "https://apt.llvm.org/llvm-snapshot.gpg.key"}], ["clang-9", "libstdc++-8-dev"])},
	"g++-5": {"env": ["CC=gcc-5 CXX=g++-5"], "compiler": "g++-5"},
	"g++-6": {"env": ["CC=gcc-6 CXX=g++-6"], "compiler": "g++-6", "addons": addon_apt([],["g++-6"])},
	"g++-7": {"env": ["CC=gcc-7 CXX=g++-7"], "compiler": "g++-7", "addons": addon_apt([],["g++-7"])},
	"g++-8": {"env": ["CC=gcc-8 CXX=g++-8"], "compiler": "g++-8", "addons": addon_apt([],["g++-8"])},
	"g++-9": {"env": ["CC=gcc-9 CXX=g++-9"], "compiler": "g++-9", "addons": addon_apt([],["g++-9"])},

	"task.coverity": {"env": ["TASK=coverity"]},
	"task.sonarcloud": {"env": ["TASK=sonarcloud"], "addons": addon_cache("$HOME/usr/", "$HOME/.sonar/cache", "build/", "bw-output/")},
	"task.sonarcloud-build": {"script": ["TASK=sonarcloud-build"]},
	"task.sonarcloud-scan": {"script": ["TASK=sonarcloud-scan"]},
	"task.clang-ubsan": {"env": ["TASK=clang-ubsan"]},
	"task.documentation": {"if": "branch = master", "env": ["TASK=documentation"], "addons": addon_apt([],["ghostscript", "graphviz", "libclang-7-dev", "lmodern", "texinfo", "texlive", "texlive-font-utils", "texlive-latex-extra", "texlive-plain-generic", "python3-pip", "python3-setuptools"])},
	"task.pycarl": {"env": ["TASK=pycarl"], "addons": addon_apt([],["python3"])},
	"task.addons": {"env": ["TASK=addons"]},
	"task.tidy": {"env": ["TASK=tidy"]},

	"j1": {"script": ["MAKE_PARALLEL=-j1"]},
	"build.sh": {"script": ["source .ci/build.sh"]},
	"mayfail": {"allow_failure": True},
}

def propertymapper(j):
	"""Merges the properties of a job into the job itself."""
	if "properties" not in j: return j
	res = j.copy()
	for p in j["properties"]:
		res = datamerge(res, properties[p])
	return res

def render_template(jobs, cached, template = "travis.yml.tpl", filename = "../.travis.yml"):
	"""Applies the propertymapper to the list of jobs and renders the jobs into the given template."""
	jobs = map(propertymapper, jobs)
	jobs = sorted(jobs, key = lambda x: (-len(x["stage"]),x["priority"]))
	
	j2_env = Environment(loader = FileSystemLoader(os.path.dirname(__file__)), keep_trailing_newline = True)
	tpl = j2_env.get_template(template)
	res = tpl.render(cached = cached, jobs = jobs)

	open(filename, "w").write(res)

def job(priority, properties):
	"""Creates a new job with the given priority and properties."""
	return {
		"priority": priority,
		"properties": properties,
	}
