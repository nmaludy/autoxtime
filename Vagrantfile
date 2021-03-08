# -*- mode: ruby -*-
# vi: set ft=ruby :

# Description:
#   This is a Vagrant file for developers to quickly get started with development
#   on the puppet-st2 module.
#
# Usage:
#   - Install VirtualBox (https://www.virtualbox.org/manual/ch02.html)
#     - OR Install KVM/libvirt (https://www.linuxtechi.com/install-kvm-hypervisor-on-centos-7-and-rhel-7/)
#   - Install Vagrant (https://www.vagrantup.com/docs/installation/)
#
#   - Start vagrant VM
#       vagrant up
##
#   - Login to vagrant VM
#       vagrant ssh
#
#   - Destroy VM
#       vagrant destroy -f

# hostname of the VM
hostname = 'autoxtime'

# We also support the :libvirt provider for CentOS / RHEL folks
provider = ENV['PROVIDER'] || 'libvirt'
provider = provider.to_sym

# The following boxes will work for both :virtualbox and :libvirt providers
#  - centos/7
# box = ENV['BOX'] || 'centos/stream8'
box = ENV['BOX'] || 'peru/windows-server-2019-standard-x64-eval'

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = 2

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.define hostname do |vm_cfg|
    # Box details
    vm_cfg.vm.box = box
    vm_cfg.vm.hostname = hostname

    # Box Specifications
    if provider == :virtualbox
      vm_cfg.vm.provider :virtualbox do |vb|
        vb.name = hostname
        vb.memory = 2048
        vb.cpus = 2
        vb.customize ['modifyvm', :id, '--uartmode1', 'disconnected']
      end
    elsif provider == :libvirt
      vm_cfg.vm.provider :libvirt do |lv|
        lv.host = hostname
        lv.memory = 2048
        lv.cpus = 2
        lv.uri = 'qemu:///system'
        lv.storage_pool_name = 'default'
      end
    else
      raise "Unsupported provider: #{provider}"
    end
  end
end
