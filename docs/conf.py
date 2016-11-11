from __future__ import print_function
import os
import os.path as path
import subprocess

doxygen_output_dir = path.abspath(
                        os.environ.get('DOXYGEN_OUTPUT_DIR', './doxygen'))

code_source_dir = path.abspath(
                    os.environ.get('CODE_SOURCE_DIR',
                                   path.join(path.dirname(__file__),
                                             '..',
                                             'include',
                                             'observable')))

docs_source_dir = path.abspath(path.dirname(__file__))

# Sphinx configuration

project = "Observable"
master_doc = 'index'

extensions = ['breathe']
breathe_projects = {
    'observable': doxygen_output_dir
}

breathe_default_project = 'observable'
breathe_domain_by_file_pattern = {
    '*': 'cpp',
}

# Doxygen configuration

def run_doxygen(app):
    """Run Doxygen over the library source."""
    print("Running Doxygen. Input dir is '%s'. Output dir is '%s'." % 
          (code_source_dir, doxygen_output_dir))

    doxygen_conf = [
        'PROJECT_NAME = "%s"' % project,
        'GENERATE_XML = YES',
        'INPUT = %s' % code_source_dir,
        'OUTPUT_DIRECTORY = %s' % doxygen_output_dir,
        'XML_OUTPUT = %s' % doxygen_output_dir,
        'RECURSIVE = YES',
        'GENERATE_HTML = NO',
        'GENERATE_LATEX = NO',
        'QUIET = YES',
    ]

    proc = subprocess.Popen(['doxygen', '-'],
                            stdin=subprocess.PIPE,
                            universal_newlines=True)
    proc.communicate('\n'.join(doxygen_conf))
    proc.wait()

def setup(app):
    # Run doxygen after Sphinx is initialized.
    app.connect('builder-inited', run_doxygen)